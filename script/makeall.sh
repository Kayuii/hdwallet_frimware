#!/bin/bash
export ROOT=$(cd "$(dirname "$0")"; pwd)/..
export SDK_ROOT=/Users/gecko/ARMSDK/nRF52/15_2_0
# export OUTPUT_DIRECTORY=$(ROOT)/ses/Output/Debug/Exe/
export OUTPUT_DIRECTORY=$ROOT/_build
echo $ROOT
echo $OUTPUT_DIRECTORY
rm -rf $OUTPUT_DIRECTORY/
cd $ROOT/ble_app/armgcc
make -j8
cd $ROOT/bootloader/armgcc
make -j8
cd $ROOT/

mergehex -m $SDK_ROOT/components/softdevice/s140/hex/s140_nrf52_6.1.0_softdevice.hex \
            $OUTPUT_DIRECTORY/nrf52840_xxaa_s140.hex \
            $OUTPUT_DIRECTORY/nrf52840_xxaa.hex \
         -o $OUTPUT_DIRECTORY/nrf52840_xxaa_full.hex

nrfjprog -f nrf52 --recover
nrfjprog -f nrf52 --eraseall
nrfjprog -f nrf52 --program $OUTPUT_DIRECTORY/nrf52840_xxaa_full.hex
nrfjprog -f nrf52 --rbp ALL