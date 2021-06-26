## Sensehat i2c Protocol
The sensors and i/o devices on the sensehat communicate with the raspberry pi using the i2c protocol. To be specific they actually
use the System Management Bus (SMB) protocol which is a subset of i2c that is more strictly structured. Each of the environmental sensors
has their own set of i2c/smb behavior that is documented in their datasheets and supported by the drivers that the manufacturers provide
that are loaded automatically at startup because of the device tree blob. The joystick and led matrix however, are custom and are controlled
by a single microproccessor and this driver is responsible for communicating with that microproccessor and providing support for the joystick and
framebuffer. The sensehat microprocessor presents 256 one byte registers that can be read from and some written to. 

The framebufffer is represented as 192 r/w registers from address 0 to 191 (0xBF). Each row of LEDs is represented by 24 bytes, the first 8 holding
the R value from 0 to 31 for each pixel, the next 8 holding the G value from 0 to 31 and the last 8 holding the B value from 0 to 31. Each row
has 24 bytes and so the 8 rows in total sum to the 192 registers. 

The joystick is represented as a single read only register with address 242 (0xF2). Each of the least significant 5 bits represents the state of
one of the joystick buttons.

The ID eeprom that raspberry pi os uses for auto loading the drivers is write protected by default but this is controlled by a r/w register with address
243 (0xF3). The default value of 0 means the eeprom is write protected, but a value of 1 can be written to this register to disable the write protection
if the eeprom needs to be reflashed

Finally there are two read only registers that are used to identify the sensehat and its firmware version. Address 190 (0xF0) contains 's' (0x73) for sensehat
which the driver verifies as a sanity check during probe and address 191 (0xF1) contains the firmware version (currently 0). This is unused at the moment
but could be used to distinguish an updated firmware from the stock one.

### Example Commands

The registers for the device can be experimented with using the CLI tools in the i2c-tools package. To read from a register you can use `i2cget` and to
write to a register you can use `i2cset`. The sensehat microcontroller uses i2c address 70 (0x46) and should be found on i2c bus 1.

 - `sudo i2cget -y 1 0x46 0xF0` this will read the "who am I" register from the sensehat and should print 0x73 for ascii 's'

Assuming the display is still showing the default rainbow pattern as when powered on but before the driver has been loaded:
 - `sudo i2cget -y 1 0x46 0x00` this will read from the red channel of the first pixel and should return the maximum value of 0xf1 as that pixel is bright red
 - `sudo i2cset -y 1 0x46 0x00 0x00` this will set the red value of the first pixel to 0 turning it completely off
