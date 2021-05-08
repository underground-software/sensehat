This folder contains a test program (hopefully more than one in the future) that you can use to test the framebuffer and joystick modules.

compile by running:

`$ make`

### `framebuffer_test`:
 - This program will display a single light on the screen that can be moved with the arrow keys (or the joystick)
 - Each time a pixel is illuminated it gets a random color 
 - Press enter (or click the joystick) to exit the program
 - The code depends on ncurses which may or may not be installed by default
 - By default the program will try to open `/dev/fb1` which is usually the Sense HAT framebuffer, but an alternative path can be passed as an argument
 - The user who runs this code must be able to open the frame buffer device file. Root can certainly do it, though usually there is a video group that the user can be added to to let non super users run this
