@echo on
adb reboot bootloader
fastboot flash userdata userdata_backup.img
fastboot reboot

pause