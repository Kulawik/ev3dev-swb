on target:
	gdbserver host_ip:3333 hello
	e.g. gdbserver 192.168.164.123:3333 hello
on host:
	gcc -g -o hello hello.c
	scp or sftp ...
	
	gdb -tui hello
	in gdb:
		target remote target_ip:3333

	to close gdbserver from host gdb:
		monitor exit

