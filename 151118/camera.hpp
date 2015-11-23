#ifndef _CAMERA_
#define _CAMERA_ 

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Camera {
	public:
		VideoCapture cap;
		int cameraSrc;

		Camera() {
		}

		Camera(int webCamera) {
			cameraSrc = webCamera;
			cap = VideoCapture(webCamera);
		}

		~Camera() {
			cap.release();
		}

		void getImage(Mat &img) {
			cap >> img;
		}
};

#endif