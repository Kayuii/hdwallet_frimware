#!/bin/bash
cd armgcc
make -j8
cd ..
nrfutil pkg generate --hw-version 52 --sd-req 0xb7 --application-version 0x01 --application ./armgcc/_build/nrf52840_xxaa.hex --key-file ../../keys/private.pem app_dfu_package_611.zip 
