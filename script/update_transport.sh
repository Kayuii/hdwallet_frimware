#!/bin/bash

protoc -I../device-protocol -I../include/transport --plugin=nanopb=../thirdparty/nanopb/generator/protoc-gen-nanopb "--nanopb_out=-f ../include/transport/types.options:." types.proto
mv types.pb.c ../source/transport/
mv types.pb.h ../include/transport/

protoc -I../device-protocol -I../include/transport --plugin=nanopb=../thirdparty/nanopb/generator/protoc-gen-nanopb "--nanopb_out=-f ../include/transport/messages.options:." messages.proto
mv messages.pb.c ../source/transport/
mv messages.pb.h ../include/transport/

protoc -I../device-protocol -I../include/transport --plugin=nanopb=../thirdparty/nanopb/generator/protoc-gen-nanopb "--nanopb_out=-f ../include/transport/exchange.options:." exchange.proto
mv exchange.pb.c ../source/transport/
mv exchange.pb.h ../include/transport/

