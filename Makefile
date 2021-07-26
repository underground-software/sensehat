<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
obj-m += rpisense-core.o rpisense-js.o rpisense-fb.o

.PHONY: build clean load
=======
=======
>>>>>>> ed899c1 (framebuffer testfile)
=======
>>>>>>> 5ea0f9a (char device to replace framebuffer)
=======
>>>>>>> c5aa68f (accessing plaltform device through misdevice.parent)
=======
>>>>>>> 067e75a (commit to merge with master)
<<<<<<< HEAD
obj-m += rpisense-core.o rpisense-fb.o rpisense-js.o
=======
obj-m += rpisense-core.o rpisense-cd.o rpisense-js.o
>>>>>>> 6a892bc (rpisense-cd.c with read functionality)
<<<<<<< HEAD
>>>>>>> 639469e (rpisense-cd.c with read functionality)
=======
=======
=======
>>>>>>> cc74e0b (char device to replace framebuffer)
=======
>>>>>>> 2615cec (accessing plaltform device through misdevice.parent)
obj-m += rpisense-core.o rpisense-cd.o rpisense-js.o
=======
obj-m += rpisense-core.o rpisense-fb.o rpisense-js.o
>>>>>>> b54532f (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ea10db8 (framebuffer testfile)
<<<<<<< HEAD
>>>>>>> ed899c1 (framebuffer testfile)
=======
=======
=======
obj-m += rpisense-core.o rpisense-cd.o rpisense-js.o
>>>>>>> b9a4fe0 (char device to replace framebuffer)
<<<<<<< HEAD
>>>>>>> cc74e0b (char device to replace framebuffer)
<<<<<<< HEAD
>>>>>>> 5ea0f9a (char device to replace framebuffer)
=======
=======
=======
obj-m += rpisense-core.o rpisense-display.o rpisense-js.o
>>>>>>> 107da44 (accessing plaltform device through misdevice.parent)
>>>>>>> 2615cec (accessing plaltform device through misdevice.parent)
<<<<<<< HEAD
>>>>>>> c5aa68f (accessing plaltform device through misdevice.parent)
=======
=======
obj-m += rpisense-core.o rpisense-display.o rpisense-js.o
>>>>>>> 3bae669 (commit to merge with master)
>>>>>>> 067e75a (commit to merge with master)

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
