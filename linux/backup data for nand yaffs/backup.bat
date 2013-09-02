@echo on
adb remount
adb push mkyaffs2image /system/bin
adb shell chmod 755 /system/bin/mkyaffs2image
adb shell mkyaffs2image -c 2048 -s 64 /data /sdcard/userdata_backup.img 
adb shell rm /system/bin/mkyaffs2image
pause