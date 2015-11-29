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
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <vector>

#define fontFace FONT_HERSHEY_PLAIN
#define PI 3.14159

using namespace cv;
using namespace std;

class Detector{
	public:
		Detector();
		void detect(Mat frame, Mat& bookImg, int& pageNum, Point& relpoint);
		Point abs2rel(Point p);
		Point rel2abs(Point p);

	private:
		float angle;
		Point bookCoord;
		tesseract::TessBaseAPI *api;
		void cropBook(Mat src, Mat& bookImg, float angle, Mat mask);
		void detectBook(Mat src, Point &leftTop, float& angle, Mat& bookArea);
		Point detectTip(Mat binary);
		void filter(Mat src, Mat& dst);
		int getPageNum(Mat src);
		float getAngle(Point s, Point f, Point e);
		void rotate(Mat src, Mat& dst, float angle);
		Point getAbsPoint(Point p, Point leftTop, float angle);
		int findBiggestContour(vector<vector<Point> > contours);
		void findStippest(vector<Point> polygon, Point& stippest);
		void findBookRegion(vector<Point> cHull, Point& leftTopP, Point& rightTopP);
};

#endif