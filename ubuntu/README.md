### 可能用到的软件:

	nautilus
	tmux
	vim+ctags
	wget

### NFS启动的共享配置 

add below to ``/etc/exportfs`` tail:

	/srv/nfsroot/ 192.168.42.0/24(rw,sync,no_root_squash,no_subtree_check)
	
then, run below commends:

	$ sudo exportfs -r
	$ sudo tunctl -u $USER -t tap0
	$ sudo ifconfig tap0 192.168.42.1
