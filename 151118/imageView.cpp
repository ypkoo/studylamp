#include "ImageView.hpp"

using namespace cv;

ImageView::ImageView(const char *_name, int _width, int _height, int xpos, int ypos) :
name(_name){
	namedWindow(_name, CV_WINDOW_AUTOSIZE);

	if (width != -1 || height != -1)
		resizeWindow(_name, _width, _height);
	if (xpos != -1 || ypos != -1)
		moveWindow(_name, xpos, ypos);

	name = _name;
	width = _width;
	height = _height;
}

void ImageView::showImage(Mat& img) {
	imshow(name, img);
}