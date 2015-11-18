from image_view import ImageView, handle_quit
import cv2
import time
import numpy as np

iv = ImageView("test")
img = np.zeros((640, 480, 3), np.uint8)

pressing = False
tuples = [] # (x, y, t)

def mouseHook(event, x, y, flags, param):
	global pressing, tuples, img, iv
	if event == cv2.EVENT_LBUTTONDOWN:
		pressing = True

	elif event == cv2.EVENT_MOUSEMOVE:
		if pressing:
			tuples.append((x, y, time.time()))
			cv2.rectangle(img, (x-1, y-1), (x+1, y+1), (255, 0, 0))

	elif event == cv2.EVENT_LBUTTONUP:
		pressing = False

	else:
		return
	
	iv.showImage(img)

cv2.setMouseCallback("test", mouseHook)
while True:

	handle_quit()