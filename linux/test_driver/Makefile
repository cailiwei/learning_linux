# Author: zhiqiang.xu
# EMail : zhiqiang.xu@phicomm.com.cn
# Date  : 2012-12-11
CROSS_ARCH:=ARCH=arm CROSS_COMPILE="$(ARM_EABI_TOOLCHAIN)/arm-eabi-"
KDIR:=$(ANDROID_PRODUCT_OUT)/obj/KERNEL_OBJ/
PWD:=$(shell pwd)

test_driver-objs := module_driver.o ft5x06_ts.o focaltech_ctl.o  ft5x06_ex_fun.o
obj-m:= test_driver.o
.PHONY: modules package clean
all:package
modules:
	@if [ "$(ANDROID_BUILD_TOP)" = "" ]; then echo "You have to run \". build/envsetup.sh\" to init enviroment first. \nAnd then you have to run \"choosecombo\" to setup the project."&&exit 1; fi
	@if [ ! -d $(KDIR) ]; then echo "Build kernel first."&&cd $(ANDROID_BUILD_TOP)&&make -j4 bootimage&&cd -; fi
	$(MAKE) $(CROSS_ARCH) -C $(KDIR) M=$(PWD) modules

package:modules
	@mkdir -p ./package
	@cp *.bat ./package
	@cp $(obj-m:.o=.ko) ./package
	@tar --transform='s,package,test_driver,' -jcf test_driver.tar.bz2 ./package/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers package test_driver.tar.bz2
