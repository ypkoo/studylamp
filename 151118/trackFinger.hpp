#ifndef _TRACK_FINGER_HPP_
#define _TRACK_FINGER_HPP_

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui_c.h"
#include <vector>

#define fontFace FONT_HERSHEY_PLAIN
#define PI 3.14159

using namespace cv;
using namespace std;

class TrackFinger{
	public:
		TrackFinger(bool _debug = false);
		Mat original;

		Point getFingerPoint(Mat img);

	private:
		bool debug;
		int c_lower[3];
		int c_upper[3];
		Mat binary;
		Rect bRect;
		void visualize();
		void produceBinaries();
		int findBiggestContour(vector<vector<Point> > contours);
		float getAngle(Point s, Point f, Point e);
};

#endif
