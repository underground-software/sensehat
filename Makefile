obj-m += rpisense-core.o rpisense-js.o rpisense-display.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load:
	sudo insmod rpisense-core.ko
	sudo insmod rpisense-js.ko
	sudo insmod rpisense-display.ko
unload:
	-sudo rmmod rpisense_js rpisense_display rpisense_core
