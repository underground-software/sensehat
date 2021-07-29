obj-m += rpisense-core.o rpisense-display.o rpisense-js.o

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load: build
	sudo insmod rpisense-core.ko
	sudo insmod rpisense-js.ko
	sudo insmod rpisense-fb.ko
