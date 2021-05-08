obj-m += rpisense-core.o rpisense-js.o rpisense-fb.o

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load: build
	sudo modprobe sysimgblt
	sudo modprobe sysfillrect
	sudo modprobe syscopyarea
	sudo modprobe fb_sys_fops
	sudo insmod rpisense-core.ko
	sudo insmod rpisense-js.ko
	sudo insmod rpisense-fb.ko
