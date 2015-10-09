"""
This file is not necessary for this project directly.
However this module would help you to debug codes.
"""


import cv, cv2
import sys

def handle_quit(delay=10):
	""" If you press 'q' key or 'esc' key, program would be terminated """
	key = cv2.waitKey(delay)
	key &= 0xFF
	if key in [ord('q'), ord('Q'), 27]:
		sys.exit(0)

class ImageView:

	_applied_names = []
	def __init__(self, name, width = -1, height = -1, xpos = -1, ypos = -1):
		"""
		* ImageView Class. Used to see image by window with real time.

		* Initialized by Name, Width, and Height.
		* Name should be unique, according to limits of cv2.
		* If Width and Height are not initialized,
		     the window resizes itself corresponding to given image.
		"""
		if name in ImageView._applied_names:
			sys.stderr.write("Failed to apply window with name %s." % name)
			sys.exit(1)
		
		ImageView._applied_names.append(name)
		cv2.namedWindow(name, cv.CV_WINDOW_AUTOSIZE)

		if width != -1 or height != -1:
			cv2.resizeWindow(name, width, height)

		if xpos == -1 and ypos == -1:
			cv2.moveWindow(name, len(ImageView._applied_names) * 10,  \
				len(ImageView._applied_names) * 10)
		else:
			cv2.moveWindow(name, xpos, ypos)

		self.name = name
		self.width = width
		self.height = height

	def showImage(self, img):
		if len (img.shape) == 2:
			w, h = img.shape
		else:
			w, h, _ = img.shape

		if self.width < w or self.height < h: # If window size is small
			self.width = max(self.width, w)
			self.height = max(self.height, h)
			cv2.resizeWindow(self.name, self.width, self.height)

		cv2.imshow(self.name, img)

if __name__ == "__main__":
	from camera import Camera
	#imageView = ImageView("Camera", 640, 480)
	imageView = ImageView("Camera")
	#imageView_2  = ImageView("Camera_2", 100, 100)
	imageView_2  = ImageView("Camera_2")

	camera = Camera(0)

	while True:
		image = camera.getImage()
		imageView.showImage(image)
		imageView_2.showImage(image[100:200, 100:200])
		handle_quit()

