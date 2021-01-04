#!/bin/bash
mkdir -p obj
pushd obj
cmake -DCMAKE_BUILD_TYPE=Release ../src
make
popd
