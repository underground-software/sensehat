how to compile and load the sensehat drivers for raspberry pi 4 on fedora

clone the repository to get a copy of the code on your device, and then code can be compiled by running

`$ make`

and the modules (and their dependancies) can be loaded with

`$ make load`

Note: the makefile simply depends on the kernel makefile and if the kernel source is not installed / downloaded on the device make will not even be able to compile the module.
If you have built your own kernel from source that you are running, it should already be in the correct place such that you can skip this step,
but the default fedora image I was testing this on did not come with its kernel source directory populated and so I had to install the following packages: 

`$ sudo dnf install kernel-devel kernel-tools kernel-headers`

Once those packages were installed the correct source files were loaded so that the makefile worked as intended

Final Note: The devices may not be properly detected even though the modules load correctly. I ran into this issue and it turns
out that I had not enabled device tree for discovering the hardware. Once I turned it on the drivers worked as expected. To do this:
Press escape during the boot process to get into the settings menu and then navigate to Device manager -> Raspberry Pi Configuration -> Advanced Configuration
and make sure that System Table Selection is set to ACPI + Devicetree
