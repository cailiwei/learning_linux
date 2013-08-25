#!/bin/bash
# set build env
source $HOME/qemu/toolschain/envcrosstool.sh
make ARCH=arm clean
make ARCH=arm mini2440_config
make ARCH=arm
echo "-------------------------------------"
echo "Build successfully !!"
cp ./u-boot.bin ../temp/ 

