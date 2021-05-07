#!/usr/bin/python
import time
from sense_hat import SenseHat

sense = SenseHat()
sense.clear()

msleep = lambda x: time.sleep(x/ 1000.0)

while True:

	sense.load_image("space_invader.png")
	msleep(.2)
