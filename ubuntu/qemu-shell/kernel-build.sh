#!/bin/bash
# build kernel for mini2440: uImage
source $HOME/qemu/toolschain/envcrosstool.sh
export PATH=$PATH:$HOME/qemu/u-boot/tools
# make ARCH=arm clean
make ARCH=arm mini2440_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux- uImage | tee build.info
echo "-------------------------------------------"
echo "Build successfully !!"
cp arch/arm/boot/uImage ../temp/
