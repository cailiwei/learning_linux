@echo off
REM test driver script
REM Author:zhiqiang.xu
REM Email :zhiqiang.xu@phicomm.com.cn
REM Date  :2012-12-11

set ADAP=1
set ADDR=0x38
set MODULE_FILE=test_driver
set DEST="/data/"

adb push %MODULE_FILE%.ko %DEST%/ 2>nul 1>nul
adb shell "svc power stayon true&&sleep 2"
adb shell "insmod %DEST%/%MODULE_FILE%.ko adap=%ADAP% addr=%ADDR%"
echo press any key to remove the driver.
pause
adb shell rmmod %MODULE_FILE%
adb shell rm %DEST%/%MODULE_FILE%.ko
adb shell svc power stayon false
@echo on
