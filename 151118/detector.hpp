#ifndef _DETECTOR_HPP_
#define _DETECTOR_HPP_

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

class Detector{
	public:
		Detector();
		void detectBook(Mat frame, Mat& dst, Mat& pageImg);
		Point detectTip(Mat frame);

	private:
		float getAngle(Point s, Point f, Point e);
		int findBiggestContour(vector<vector<Point> > contours);
		void findStippest(vector<Point> polygon, Point& stippest);
		void produceBinaries(Mat frame, Mat& dst, Scalar lowerBound, Scalar upperBound);
		
		Point RotatePoint(const Point2f& p, float rad);
		Point RotatePoint(Point cen_pt, Point p, float rad);
		void rotate(Mat src, double angle, Mat& dst);
		float distanceP2P(Point a, Point b);
};

#endif
