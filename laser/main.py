from laser_tracker import LaserTracker
from camera import Camera
from ocr import demo_word_ocr_cv
import time

laserTracker = LaserTracker(20, 160, 100, 255, 200, 255)
camera = Camera(0, 640, 480)

def func(img, x, y, w, h):
	"""
	* In laserTracker.run()
	*    this function would be called if laser is detected with point, w, h.

	* @TODO Make exact roles of Lasertracker and OCR.
	* How to communicate two objects?
	
	* laser point detection (done by LaserTracker)
	* -> make rectangle including words (what object should do?)
	* -> picture to word (success or fail)
	*          Q. Is it have to send a message to the LaserTracker if failure?
	"""
	ret = demo_word_ocr_cv(img, (x, y), w, h)
	if ret:
		time.sleep (1.5)

laserTracker.setFunc(func)


def image_test():
	from image_view import ImageView, handle_quit
	iv = ImageView("original", 640, 480, 20, 0)
	iv2 = ImageView("filtered", 640, 480, 660, 0)

	while True:
		img = camera.getImage()
		filtered_img = laserTracker.getFilteredImage(img)
		iv.showImage(img)
		iv2.showImage(filtered_img)
		handle_quit()

def main():
	while True:
		laserTracker.run(camera.getImage())

if __name__ == "__main__":
	image_test()