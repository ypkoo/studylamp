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
		TrackFinger();
		Point getFingerPoint(Mat frame);

	private:
		int c_lower[3];
		int c_upper[3];
		float getAngle(Point s, Point f, Point e);
		int findBiggestContour(vector<vector<Point> > contours);
		void findStippest(vector<Point> cHull, Point& stippest);
		void produceBinaries(Mat original, Mat& binary);
};

#endif
