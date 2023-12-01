#!/bin/bash

protoc -I protos/ --grpc_out=src/ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` protos/station.proto
protoc -I protos/ --cpp_out=src/ protos/station.proto
