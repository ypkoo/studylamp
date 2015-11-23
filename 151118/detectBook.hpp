#ifndef _DETECT_BOOK_HPP_
#define _DETECT_BOOK_HPP_

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui_c.h"
#include <vector>

#define PI 3.14159

using namespace cv;
using namespace std;

class DetectBook{
	public:
		DetectBook();
		Mat original;
		Mat originalLRLR;
		Mat contrastLRLR;
		Mat contrastLRLRLR;

		int getPageNum();
		Mat cropBook();
		void setImage(Mat img);

	private:
		vector<vector<Point> > hullP;
		int cIdx;
		int pageNum;
		bool debug;
		Mat binary;
		vector<Mat> binaryList;
		Rect bRect;
		void produceBinaries();
		int findBiggestContour(vector<vector<Point> > contours);
		
		Point RotatePoint(const cv::Point2f& p, float rad);
		Point RotatePoint(Point cen_pt, Point p, float rad);
		void rotate(Mat src, double angle, cv::Mat& dst);

		float distanceP2P(Point a, Point b);
};

#endif
