#!/bin/bash
cd ../ble_app/armgcc
make -j8
cd ../../
pwd
nrfutil pkg generate --hw-version 52 \
    --sd-req 0xAE \
    --application-version 0x01 \
    --application ./ses/Output/Debug/Exe/ble_app.hex \
    --key-file ./keys/privkey_debug.pem \
    app_dfu_package_ses_610.zip 