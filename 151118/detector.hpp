/**
 * detector.hpp
 *
 * Header of detector.cpp
 *
 * @author Seunghyo Kang, Jongchan Park
 *
 * @version 1.0
 * @since 2015-11-23
 * First implementation
 */

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
		void detectBook(Mat frame, Mat& bookImg, Mat& pageImg);
		Point detectTip(Mat frame);

	private:
		float getAngle(Point s, Point f, Point e);
		int findBiggestContour(vector<vector<Point> > contours);
		void findStippest(vector<Point> polygon, Point& stippest);
		void findBookRegion(vector<Point> cHull, vector<Point>& endPoints);
		void rotate(Mat src, Point cen_pt, float width, float height, double angle, Mat& dst);
		float distanceP2P(Point a, Point b);
};

#endif