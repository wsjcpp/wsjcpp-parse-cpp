#!/bin/bash

check_ret() {
    if [ $1 -ne 0 ]; then
        echo ""
        echo "[X] FAIL: $2"
        echo ""
        exit $1
    else
        echo ""
        echo "[V] SUCCESS: $2"
        echo ""
    fi
}

if [ ! -d tmp ]; then
    mkdir -p tmp
fi

cd tmp
cmake ..
check_ret $? "cmake configure"
make
check_ret $? "make"