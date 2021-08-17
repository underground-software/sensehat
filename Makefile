obj-m += rpisense-core.o rpisense-joystick.o rpisense-display.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load:
	sudo insmod rpisense-core.ko
	sudo insmod rpisense-joystick.ko
	sudo insmod rpisense-display.ko
unload:
	-sudo rmmod rpisense_joystick rpisense_display rpisense_core
