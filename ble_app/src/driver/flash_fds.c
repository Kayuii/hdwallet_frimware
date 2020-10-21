#include "driver/flash_fds.h"
#include "FreeRTOS.h"
#include "board/memory.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "task.h"

/* Array to map FDS return values to strings. */
char const* fds_err_str[] = {
    "FDS_SUCCESS",
    "FDS_ERR_OPERATION_TIMEOUT",
    "FDS_ERR_NOT_INITIALIZED",
    "FDS_ERR_UNALIGNED_ADDR",
    "FDS_ERR_INVALID_ARG",
    "FDS_ERR_NULL_ARG",
    "FDS_ERR_NO_OPEN_RECORDS",
    "FDS_ERR_NO_SPACE_IN_FLASH",
    "FDS_ERR_NO_SPACE_IN_QUEUES",
    "FDS_ERR_RECORD_TOO_LARGE",
    "FDS_ERR_NOT_FOUND",
    "FDS_ERR_NO_PAGES",
    "FDS_ERR_USER_LIMIT_REACHED",
    "FDS_ERR_CRC_CHECK_FAILED",
    "FDS_ERR_BUSY",
    "FDS_ERR_INTERNAL",
};

/* Array to map FDS events to strings. */
static char const* fds_evt_str[] = {
    "FDS_EVT_INIT",       "FDS_EVT_WRITE",    "FDS_EVT_UPDATE",
    "FDS_EVT_DEL_RECORD", "FDS_EVT_DEL_FILE", "FDS_EVT_GC",
};

static bool volatile m_pending_init = false;
static bool volatile m_pending_write = false;
static bool volatile m_pending_delete = false;
static bool volatile m_pending_gc = false;

static bool volatile m_write_error = false;

#define IS_OPERATION_PENDING \
    (m_pending_write || m_pending_delete || m_pending_gc)

#define WAIT_PENDING_FLAG(x) \
    {                        \
        while (x) {          \
            vTaskDelay(10);  \
        }                    \
    }

static void fds_evt_handler(fds_evt_t const* p_fds_evt) {
    NRF_LOG_INFO("Event: %s received (%s)", fds_evt_str[p_fds_evt->id],
                 fds_err_str[p_fds_evt->result]);

    switch (p_fds_evt->id) {
        case FDS_EVT_INIT: {
            if (p_fds_evt->result == FDS_SUCCESS) {
                m_pending_init = false;
            }
        } break;
        case FDS_EVT_WRITE: {
            if (p_fds_evt->result == FDS_SUCCESS) {
                NRF_LOG_INFO(
                    "Record ID:\t0x%04x File ID:\t0x%04x Record key:\t0x%04x",
                    p_fds_evt->write.record_id, p_fds_evt->write.file_id,
                    p_fds_evt->write.record_key);
                m_write_error = false;
            } else {
                m_write_error = true;
            }
            m_pending_write = false;
        } break;
        case FDS_EVT_UPDATE: {
            if (p_fds_evt->result == FDS_SUCCESS) {
                m_write_error = false;
                NRF_LOG_INFO(
                    "Record ID:\t0x%04x File ID:\t0x%04x Record key:\t0x%04x",
                    p_fds_evt->write.record_id, p_fds_evt->write.file_id,
                    p_fds_evt->write.record_key);
            } else {
                m_write_error = true;
            }
            m_pending_write = false;
        } break;
        case FDS_EVT_DEL_RECORD: {
            if (p_fds_evt->result == FDS_SUCCESS) {
                NRF_LOG_INFO(
                    "Record ID:\t0x%04x File ID:\t0x%04x Record key:\t0x%04x",
                    p_fds_evt->del.record_id, p_fds_evt->del.file_id,
                    p_fds_evt->del.record_key);
            }
            m_pending_delete = false;
        } break;
        case FDS_EVT_GC: {
            if (p_fds_evt->result == FDS_SUCCESS) {
                NRF_LOG_INFO("Garbage Collector activity finished.");
            }
            m_pending_gc = false;
        } break;
        default:
            break;
    }
}

void storage_fds_init() {
    ret_code_t rc;
    m_pending_init = true;
    rc = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(rc);
    rc = fds_init();
    APP_ERROR_CHECK(rc);

    WAIT_PENDING_FLAG(m_pending_init);
#ifdef DEBUG
    fds_stat_t stat = {0};
    rc = fds_stat(&stat);
    APP_ERROR_CHECK(rc);
    NRF_LOG_INFO("Found %d valid records, %d dirty records.",
                 stat.valid_records, stat.dirty_records);
#endif
}

bool record_exists(uint16_t fid, uint16_t key) {
    fds_record_desc_t desc = {0};
    fds_find_token_t ftok = {0};
    return (fds_record_find(fid, key, &desc, &ftok) == FDS_SUCCESS);
}

void record_gc(void) {
    // If there is no space, preserve write request and call Garbage
    // Collector.
    NRF_LOG_INFO("FDS has no space left, Garbage Collector triggered!");
    if (!m_pending_gc) {
        ret_code_t rc = FDS_SUCCESS;
        do {
            m_pending_gc = true;
            rc = fds_gc();
            switch (rc) {
                case FDS_SUCCESS:
                    break;
                case FDS_ERR_NO_SPACE_IN_QUEUES:
                    vTaskDelay(10);
                    break;
                default:
                    NRF_LOG_ERROR("FDS GC Error :%d", rc);
                    return;
            }
        } while (rc != FDS_SUCCESS);
    }
    WAIT_PENDING_FLAG(m_pending_gc);
}

ret_code_t record_write(uint16_t fid, uint16_t key, void const* p_data,
                        uint32_t len) {
    NRF_LOG_INFO("Record Write: %x, len:%d", p_data, len);
    fds_record_desc_t desc = {0};
    fds_find_token_t ftok = {0};
    fds_record_t const rec = {
        .file_id = fid,
        .key = key,
        .data.p_data = p_data,
        .data.length_words = (len + 3) / sizeof(uint32_t)};

    ret_code_t rc = FDS_SUCCESS;
    bool find_record = (fds_record_find(fid, key, &desc, &ftok) == FDS_SUCCESS);
    do {
        do {
            m_pending_write = true;
            if (find_record) {
                rc = fds_record_update(&desc, &rec);
            } else {
                rc = fds_record_write(&desc, &rec);
            }

            switch (rc) {
                case FDS_SUCCESS:
                    m_write_error = false;
                    break;
                case FDS_ERR_NO_SPACE_IN_FLASH:
                    record_gc();
                    break;
                case FDS_ERR_NO_SPACE_IN_QUEUES:
                    vTaskDelay(10);
                    break;
                default:
                    NRF_LOG_ERROR("FDS Write Error :%d", rc);
                    return rc;
            }
        } while (rc != FDS_SUCCESS);
        WAIT_PENDING_FLAG(m_pending_write);
    } while (m_write_error);
    return rc;
}

ret_code_t record_delete(uint16_t fid, uint16_t key) {
    fds_find_token_t tok = {0};
    fds_record_desc_t desc = {0};
    ret_code_t rc = FDS_SUCCESS;

    if (fds_record_find(fid, key, &desc, &tok) == FDS_SUCCESS) {
        do {
            m_pending_delete = true;
            rc = fds_record_delete(&desc);
            switch (rc) {
                case FDS_SUCCESS:
                    break;
                case FDS_ERR_NO_SPACE_IN_QUEUES:
                    vTaskDelay(10);
                    break;
                default:
                    NRF_LOG_ERROR("FDS Update Error :%d", rc);
                    return rc;
            }
        } while (rc != FDS_SUCCESS);
        WAIT_PENDING_FLAG(m_pending_delete);
    } else {
        rc = FDS_ERR_NOT_FOUND;
        NRF_LOG_ERROR("error: record not found!\r\n");
    }

    return rc;
}

ret_code_t record_read(uint16_t fid, uint16_t key, void* p_data,
                       uint32_t* len) {
    if (len == NULL || p_data == NULL) {
        NRF_LOG_ERROR("Input error");
        return FDS_ERR_NULL_ARG;
    }

    fds_record_desc_t desc = {0};
    fds_find_token_t tok = {0};

    ret_code_t rc = FDS_SUCCESS;
    rc = fds_record_find(fid, key, &desc, &tok);
    if (rc == FDS_SUCCESS) {
        /* A config file is in flash. Let's update it. */
        fds_flash_record_t config = {0};

        /* Open the record and read its contents. */
        rc = fds_record_open(&desc, &config);
        if (rc != FDS_SUCCESS) {
            return rc;
        }
        if (*len < config.p_header->length_words * sizeof(uint32_t)) {
            NRF_LOG_INFO("Has not enough memory to keep data.\n");
            return NRF_ERROR_DATA_SIZE;
        }
        *len = config.p_header->length_words * sizeof(uint32_t);
        /* Copy the configuration from flash into m_dummy_cfg. */
        if (p_data != NULL) {
            memcpy(p_data, config.p_data, *len);
        }

        NRF_LOG_INFO("Config file found, fid:%02x, key:%02x.",
                     config.p_header->file_id, config.p_header->record_key);

        /* Close the record when done reading. */
        rc = fds_record_close(&desc);
    } else {
        /* System config not found; write a new one. */
        rc = FDS_ERR_NOT_FOUND;
        NRF_LOG_INFO("Record not found");
    }
    return rc;
}
