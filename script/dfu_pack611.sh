#!/bin/bash
cd ../ble_app/armgcc
make -j8
cd ../../
pwd
nrfutil pkg generate --hw-version 52 \
    --sd-req 0xb6 \
    --application-version 0x01 \
    --application ./_build/nrf52840_xxaa.hex \
    --key-file ./keys/privkey_debug.pem \
    app_dfu_package_611.zip 
