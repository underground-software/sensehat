obj-m += sensehat-core.o sensehat-joystick.o sensehat-display.o

.PHONY: build clean load unload

build:
	make -C /lib/modules/$(shell uname -r)/build modules M=$(PWD)

clean:
	make -C /lib/modules/$(shell uname -r)/build clean M=$(PWD)

load:
	sudo insmod sensehat-core.ko
	sudo insmod sensehat-joystick.ko
	sudo insmod sensehat-display.ko
unload:
	-sudo rmmod sensehat_joystick sensehat_display sensehat_core
