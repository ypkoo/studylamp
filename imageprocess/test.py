from image_view import ImageView, handle_quit
from camera import Camera

iv0 = ImageView("Cam0")
iv1 = ImageView("Cam1")
iv2 = ImageView("Cam2")

cm0 = Camera(0)
cm1 = Camera(1)

while True:
	iv0.showImage(cm0.getImage())
	iv1.showImage(cm1.getImage())
	#iv2.showImage(cm2.getImage())
	handle_quit()