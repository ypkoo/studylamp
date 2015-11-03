#!/usr/bin/env python
#
# camera.py
# 
# Implementing Camera class for control input of cameras which are connected to PC.
# Just initialize with Camera(device_num), and call for getImage() function.
#
# @author S.H.Lee
#
# @version 1.1
# @since 2015-11-03
# Add attributes for width and height.
#
# @version 1.0
# @since 2015-10-08
# First implementation

import cv, cv2
import sys

class Camera:
	"""	Camera class, used to get image from camera. """
	def __init__(self, device_num = 0, width = -1, height = -1):
		capture = cv2.VideoCapture(device_num)

		if not capture.isOpened():
			sys.stderr.write("Failed to open capture device.\n")
			sys.exit (1)

		if width != -1:
			capture.set(cv.CV_CAP_PROP_FRAME_WIDTH, width)
			self.width = width
		else:
			self.width = capture.get(cv.CV_CAP_PROP_FRAME_WIDTH)

		if height != -1:
			capture.set(cv.CV_CAP_PROP_FRAME_HEIGHT, height)
			self.height = height
		else:
			self.height = capture.get(cv.CV_CAP_PROP_FRAME_HEIGHT)

		self.capture = capture

	def getImage(self):
		"""
		* Capture the video, return the image captured by camera.
		* getImage : None -> numpy.ndarray (image object)
		
		* image object could be cropped like following code
		
		* # crop image to x=10~40 and y=20~50
		* new_img = img[10:40, 20:50, :]
		"""
		success, frame = self.capture.read()

		if not success:
			sys.stderr.write("Failed to read camera frame.\n")
			sys.exit(1)

		return frame

if __name__ == "__main__":
	camera = Camera(0)
	image = camera.getImage()
	red_image = image[:, :, 0]
	cv2.imwrite("image.jpg", image);
	cv2.imwrite("image_r.jpg", red_image);
	print ("image width = %d, height = %d, rgb %d" % (image.shape))
	print (dir (camera.getImage()))
