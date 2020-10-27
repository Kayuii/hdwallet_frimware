#!/bin/bash
ROOT=$(cd "$(dirname "$0")"; pwd)/..
SDK_ROOT=/Users/gecko/ARMSDK/nRF52/15_2_0
OUTPUT_DIRECTORY=$ROOT/_build
echo $ROOT
echo $OUTPUT_DIRECTORY
rm -rf $OUTPUT_DIRECTORY/nrf52840_xxaa
rm -rf $OUTPUT_DIRECTORY/nrf52840_xxaa.*
cd $ROOT/ble_app/armgcc
make -j8
cd $ROOT/
Temptime=$(date +%Y%m%d%H%M%S)
nrfutil pkg generate --hw-version 52 \
    --sd-req 0xae \
    --application-version 0x01 \
    --application ./_build/nrf52840_xxaa.hex \
    --key-file ./keys/private.pem \
    new_dfu_package_610_$Temptime.zip 