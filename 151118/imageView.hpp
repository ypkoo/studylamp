#ifndef _IMAGE_VIEW_HPP_
#define _IMAGE_VIEW_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

class ImageView {
private:
	std::string name;
	int width;
	int height;
public:
	ImageView(const char *_name, int _width = -1, int _height = -1, int xpos = -1, int ypos = -1);
	
	void showImage(Mat &);
};

#endif