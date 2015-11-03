#!/usr/bin/env python
#
# main.py
# 
# Main logic for image processing.
#
# @author S.H.Lee
#
# @version 1.2
# @since 2015-11-03
# Move accumulative points list structure into Motion structure.
#
# @version 1.1
# @since 2015-11-02
# Change on method to receive user's input (laser to finger)
# Implemented new logic for processing.
#
# @version 1.0
# @since 2015-10-08
# First implementation

from finger_tracker import FingerTracker
from camera import Camera
from ocr import demo_word_ocr_cv # need to modify
from motion import Motion
import time

camera = Camera(0)
finger_tracker = FingerTracker()
motion = Motion()

ocr = OCR()

'''
get finger point (by FingerTracker object)
-> accumulate information about point
-> check some motion (by Motion object)
-> if line type -> waits for finger disappeared
                -> get area for letters (by ?? object)
                -> get word (by OCR object)
'''
def loop():
	image = camera.getImage()
	new_point = finger_tracker.getFingerPoint(image)
	if new_point:
		motion.registerPoint(new_point, time.time())

	specificMotion = motion.analyze()

	if specificMotion:
		if specificMotion.type == "line":
			while !finger_tracker.getFingerPoint(camera.getImage()):
				
				# <- get letter area -> #

				ocr.getWord(camera.getImage())


if __name__ == "__main__":
	while True:
		loop()