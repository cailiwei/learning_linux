#!/bin/bash
# Build qemu for mini2440
make clean
./configure --target-list=arm-softmmu
make | tee build.info
echo "================================================"
echo "Build Success!!!"
