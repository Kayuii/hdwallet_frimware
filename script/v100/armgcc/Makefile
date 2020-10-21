PROJECT_NAME     := soter_wallet_v1
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := /Users/gecko/ARMSDK/nRF52/15_2_0
PROJ_DIR := ..
THIRDPARTY_DIR := ../../../thirdparty

$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT  := nrf52_u2f_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_freertos.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_core.c \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/app_usbd_hid.c \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic/app_usbd_hid_generic.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_string_desc.c \
  $(SDK_ROOT)/components/libraries/usbd/app_usbd_serial_num.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/external/fnmatch/fnmatch.c \
  $(SDK_ROOT)/components/libraries/hardfault/nrf52/handler/hardfault_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_power.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/components/drivers_nrf/usbd/nrf_drv_usbd.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_power_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_rng.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rng.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(PROJ_DIR)/../../source/main.c \
  $(PROJ_DIR)/../../source/board/canvas.c \
  $(PROJ_DIR)/../../source/board/confirm_sm.c \
  $(PROJ_DIR)/../../source/board/draw.c \
  $(PROJ_DIR)/../../source/board/font.c \
  $(PROJ_DIR)/../../source/board/layout.c \
  $(PROJ_DIR)/../../source/board/memory.c \
  $(PROJ_DIR)/../../source/board/msg_dispatch.c \
  $(PROJ_DIR)/../../source/board/resources.c \
  $(PROJ_DIR)/../../source/board/u2f.c \
  $(PROJ_DIR)/../../source/firmware/app_confirm.c \
  $(PROJ_DIR)/../../source/firmware/app_layout.c \
  $(PROJ_DIR)/../../source/firmware/coins.c \
  $(PROJ_DIR)/../../source/firmware/crypto.c \
  $(PROJ_DIR)/../../source/firmware/ethereum.c \
  $(PROJ_DIR)/../../source/firmware/ethereum_tokens.c \
  $(PROJ_DIR)/../../source/firmware/exchange.c \
  $(PROJ_DIR)/../../source/firmware/fsm.c \
  $(PROJ_DIR)/../../source/firmware/passphrase_sm.c \
  $(PROJ_DIR)/../../source/firmware/pin_sm.c \
  $(PROJ_DIR)/../../source/firmware/policy.c \
  $(PROJ_DIR)/../../source/firmware/qr_encode.c \
  $(PROJ_DIR)/../../source/firmware/recovery.c \
  $(PROJ_DIR)/../../source/firmware/recovery_cipher.c \
  $(PROJ_DIR)/../../source/firmware/reset.c \
  $(PROJ_DIR)/../../source/firmware/signing.c \
  $(PROJ_DIR)/../../source/firmware/storage.c \
  $(PROJ_DIR)/../../source/firmware/transaction.c \
  $(PROJ_DIR)/../../source/firmware/util.c \
  $(PROJ_DIR)/../../source/firmware/utf8.c \
  $(PROJ_DIR)/../../source/firmware/u2f_user.c \
  $(PROJ_DIR)/../../source/driver/battery.c \
  $(PROJ_DIR)/../../source/driver/button.c \
  $(PROJ_DIR)/../../source/driver/display.c \
  $(PROJ_DIR)/../../source/driver/epd_w21_c02.c \
  $(PROJ_DIR)/../../source/driver/epd_wxx_hal.c \
  $(PROJ_DIR)/../../source/driver/flash_fds.c \
  $(PROJ_DIR)/../../source/driver/gt24l24a2y_app.c \
  $(PROJ_DIR)/../../source/driver/gt24l24a2y_hal.c \
  $(PROJ_DIR)/../../source/driver/power.c \
  $(PROJ_DIR)/../../source/driver/rng.c \
  $(PROJ_DIR)/../../source/driver/soter_ble.c \
  $(PROJ_DIR)/../../source/driver/soter_service.c \
  $(PROJ_DIR)/../../source/driver/usb.c \
  $(PROJ_DIR)/../../source/transport/types.pb.c \
  $(PROJ_DIR)/../../source/transport/messages.pb.c \
  $(PROJ_DIR)/../../source/transport/exchange.pb.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/external/utf_converter/utf.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/blockwise.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/cifra_cmac.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/cifra_eax_aes.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/eax.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/gf128.c \
  $(SDK_ROOT)/external/cifra_AES128-EAX/modes.c \
  $(SDK_ROOT)/components/libraries/mpu/nrf_mpu.c \
  $(SDK_ROOT)/components/libraries/stack_guard/nrf_stack_guard.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra/cifra_backend_aes_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes_shared.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_eddsa.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_error.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hkdf.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_aes.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_aes_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_chacha_poly_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_eddsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_hmac.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_shared.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \
  $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/nrf_ble_lesc.c \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
  $(SDK_ROOT)/external/freertos/source/croutine.c \
  $(SDK_ROOT)/external/freertos/source/event_groups.c \
  $(SDK_ROOT)/external/freertos/source/portable/MemMang/heap_1.c \
  $(SDK_ROOT)/external/freertos/source/list.c \
  $(SDK_ROOT)/external/freertos/portable/GCC/nrf52/port.c \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis.c \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis_systick.c \
  $(SDK_ROOT)/external/freertos/source/queue.c \
  $(SDK_ROOT)/external/freertos/source/stream_buffer.c \
  $(SDK_ROOT)/external/freertos/source/tasks.c \
  $(SDK_ROOT)/external/freertos/source/timers.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_freertos.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \ \
  $(SDK_ROOT)/external/nano-pb/pb_common.c \
  $(SDK_ROOT)/external/nano-pb/pb_decode.c \
  $(SDK_ROOT)/external/nano-pb/pb_encode.c \
  $(THIRDPARTY_DIR)/trezor-crypto/address.c \
  $(THIRDPARTY_DIR)/trezor-crypto/base32.c \
  $(THIRDPARTY_DIR)/trezor-crypto/base58.c \
  $(THIRDPARTY_DIR)/trezor-crypto/bignum.c \
  $(THIRDPARTY_DIR)/trezor-crypto/bip32.c \
  $(THIRDPARTY_DIR)/trezor-crypto/bip39.c \
  $(THIRDPARTY_DIR)/trezor-crypto/blake256.c \
  $(THIRDPARTY_DIR)/trezor-crypto/blake2b.c \
  $(THIRDPARTY_DIR)/trezor-crypto/blake2s.c \
  $(THIRDPARTY_DIR)/trezor-crypto/cash_addr.c \
  $(THIRDPARTY_DIR)/trezor-crypto/curves.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ecdsa.c \
  $(THIRDPARTY_DIR)/trezor-crypto/groestl.c \
  $(THIRDPARTY_DIR)/trezor-crypto/hasher.c \
  $(THIRDPARTY_DIR)/trezor-crypto/hmac.c \
  $(THIRDPARTY_DIR)/trezor-crypto/memzero.c \
  $(THIRDPARTY_DIR)/trezor-crypto/nem.c \
  $(THIRDPARTY_DIR)/trezor-crypto/nist256p1.c \
  $(THIRDPARTY_DIR)/trezor-crypto/pbkdf2.c \
  $(THIRDPARTY_DIR)/trezor-crypto/rand.c \
  $(THIRDPARTY_DIR)/trezor-crypto/rc4.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ripemd160.c \
  $(THIRDPARTY_DIR)/trezor-crypto/script.c \
  $(THIRDPARTY_DIR)/trezor-crypto/secp256k1.c \
  $(THIRDPARTY_DIR)/trezor-crypto/segwit_addr.c \
  $(THIRDPARTY_DIR)/trezor-crypto/sha2.c \
  $(THIRDPARTY_DIR)/trezor-crypto/sha3.c \
  $(THIRDPARTY_DIR)/trezor-crypto/aes/aes_modes.c \
  $(THIRDPARTY_DIR)/trezor-crypto/aes/aescrypt.c \
  $(THIRDPARTY_DIR)/trezor-crypto/aes/aeskey.c \
  $(THIRDPARTY_DIR)/trezor-crypto/aes/aestab.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/curve25519-donna-32bit.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/curve25519-donna-helpers.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/curve25519-donna-scalarmult-base.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519-donna-32bit-tables.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519-donna-basepoint-table.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519-donna-impl-base.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519-keccak.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519-sha3.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/ed25519.c \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna/modm-donna-32bit.c \
  $(THIRDPARTY_DIR)/trezor-crypto/chacha20poly1305/chacha_merged.c \
  $(THIRDPARTY_DIR)/trezor-crypto/chacha20poly1305/chacha20poly1305.c \
  $(THIRDPARTY_DIR)/trezor-crypto/chacha20poly1305/poly1305-donna.c \
  $(THIRDPARTY_DIR)/trezor-crypto/chacha20poly1305/rfc7539.c \
  $(THIRDPARTY_DIR)/trezor-crypto/monero/range_proof.c \
  $(THIRDPARTY_DIR)/trezor-crypto/monero/serialize.c \
  $(THIRDPARTY_DIR)/trezor-crypto/monero/xmr.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/config \
  $(PROJ_DIR)/../../source/firmware \
  $(PROJ_DIR)/../../thirdparty \
  $(PROJ_DIR)/../../include \
  $(PROJ_DIR)/../../include/transport \
  $(SDK_ROOT)/external/nano-pb/
  $(THIRDPARTY_DIR)/trezor-crypto \
  $(THIRDPARTY_DIR)/trezor-crypto/aes \
  $(THIRDPARTY_DIR)/trezor-crypto/chacha20poly1305 \
  $(THIRDPARTY_DIR)/trezor-crypto/ed25519-donna \
  $(THIRDPARTY_DIR)/trezor-crypto/monero \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_link_ctx_manager \
  $(SDK_ROOT)/components/ble/ble_services \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/stack_info \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/cli \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/ecc \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/libraries/experimental_task_manager \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/gfx \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/libraries/hardfault/nrf52 \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/mpu \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/sdcard \
  $(SDK_ROOT)/components/libraries/sensorsim \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/components/libraries/sortlist \
  $(SDK_ROOT)/components/libraries/spi_mngr \
  $(SDK_ROOT)/components/libraries/stack_guard \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/libraries/twi_mngr \
  $(SDK_ROOT)/components/libraries/twi_sensor \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ac_rec_parser \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser \
  $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/le_oob_rec_parser \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ac_rec \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_oob_advdata \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_lib \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_msg \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/common \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/ep_oob_rec \
  $(SDK_ROOT)/components/nfc/ndef/connection_handover/hs_rec \
  $(SDK_ROOT)/components/nfc/ndef/generic/message \
  $(SDK_ROOT)/components/nfc/ndef/generic/record \
  $(SDK_ROOT)/components/nfc/ndef/launchapp \
  $(SDK_ROOT)/components/nfc/ndef/parser/message \
  $(SDK_ROOT)/components/nfc/ndef/parser/record \
  $(SDK_ROOT)/components/nfc/ndef/text \
  $(SDK_ROOT)/components/nfc/ndef/uri \
  $(SDK_ROOT)/components/nfc/t2t_lib \
  $(SDK_ROOT)/components/nfc/t2t_lib/hal_t2t \
  $(SDK_ROOT)/components/nfc/t2t_parser \
  $(SDK_ROOT)/components/nfc/t4t_lib \
  $(SDK_ROOT)/components/nfc/t4t_lib/hal_t4t \
  $(SDK_ROOT)/components/nfc/t4t_parser/apdu \
  $(SDK_ROOT)/components/nfc/t4t_parser/cc_file \
  $(SDK_ROOT)/components/nfc/t4t_parser/hl_detection_procedure \
  $(SDK_ROOT)/components/nfc/t4t_parser/tlv \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/softdevice/s140/headers \
  $(SDK_ROOT)/components/softdevice/s140/headers/nrf52 \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/external \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/external/freertos/config \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52 \
  $(SDK_ROOT)/external/freertos/portable/GCC/nrf52 \
  $(SDK_ROOT)/external/freertos/source/include \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/external/utf_converter \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/modules/nrfx/mdk \

# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/external/nrf_cc310/lib/libnrf_cc310_0.9.10.a \
  $(SDK_ROOT)/external/nrf_oberon/lib/nrf52/liboberon_2.0.5.a \
  $(THIRDPARTY_DIR)/gt24l24a2y/gt24l24a2y_nrf52840_segger.a \

# Optimization flags
OPT = -O1 -g1
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += -std=gnu99
CFLAGS += $(OPT)
CFLAGS += -DDEBUG
CFLAGS += -DDEBUG_NRF
CFLAGS += -DCONFIG_RANDOM_AES_KEY_ENABLED
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
CFLAGS += -DNRF52840_XXAA
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
#CFLAGS += -Wall -Werror -Wno-unused-function
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT

CFLAGS += -DFREERTOS
CFLAGS += -DINITIALIZE_USER_SECTIONS
CFLAGS += -DNO_VTOR_CONFIG
CFLAGS += -DPB_FIELD_32BIT
CFLAGS += -DRAND_PLATFORM_INDEPENDENT
CFLAGS += -DUSE_ETHEREUM

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DDEBUG
ASMFLAGS += -DDEBUG_NRF
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=16384
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=16384

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice recover erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s140_nrf52_6.1.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_6.1.0_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

#
recover:
	nrfjprog -f nrf52 --recover

#
erase:
	nrfjprog -f nrf52 --eraseuicr
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := $(PROJ_DIR)/config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
