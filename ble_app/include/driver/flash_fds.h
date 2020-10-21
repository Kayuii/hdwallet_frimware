#ifndef __STORAGE_FDS_H__
#define __STORAGE_FDS_H__
#include "fds.h"

void storage_fds_init();
bool record_exists(uint16_t fid, uint16_t key);
ret_code_t record_write(uint16_t fid, uint16_t key, void const* p_data,
                        uint32_t len);
ret_code_t record_read(uint16_t fid, uint16_t key, void* p_data, uint32_t* len);

ret_code_t record_delete(uint16_t fid, uint16_t key);

void record_gc();

#endif
