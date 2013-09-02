使用方法及注意事项:
1.确保解压的文件放在带有adb环境的目录下
2.确保有sdcard存在并且剩余容量>=200m
3.确保backup.bat和mkyaffs2image在同一目录下。
4.运行backup.bat脚本，data下的所有东西会被备份到sdcard下的userdata_backup.img
还原方法：
1.对于高通平台就是用fastboot，把userdata_backup.img拷贝到有fastboot环境的目录下，运行fastboot_restore_userdata.bat
2.对于marvell平台就直接用工具烧录吧，