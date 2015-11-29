/**
 * detector.cpp
 *
 * Detector class, detect a book and a pen tip from an image
 *
 * @author Seunghyo Kang, Jongchan Park
 *
 * @version 1.0
 * @since 2015-11-23
 * First implementation
 */

#include "detector.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace cv;
using namespace std;

Detector::Detector() {
	api = new tesseract::TessBaseAPI();
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	api->SetVariable("tessedit_char_whitelist","0123456789");
}

int Detector::findBiggestContour(vector<vector<Point> > contours) {
	 int indexOfBiggestContour = -1;
	 int sizeOfBiggestContour = 0;
	 for (int i = 0; i < contours.size(); i++){
		  if(contours[i].size() > sizeOfBiggestContour){
				sizeOfBiggestContour = contours[i].size();
				indexOfBiggestContour = i;
		  }
	 }
	 return indexOfBiggestContour;
}

float Detector::getAngle(Point s, Point f, Point e) {
	float l1 = sqrt(pow(f.x-s.x,2) + pow(f.y-s.y,2));
	float l2 = sqrt(pow(f.x-e.x,2) + pow(f.y-e.y,2));
	if (l1*l2 == 0)
		return PI/2;
	float dot = (s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	return 180 * acos(dot/(l1*l2)) / PI;
}

void Detector::findStippest(vector<Point> polygon, Point& stippest) {
	int n = polygon.size();
	float smallest = 360;
	if (n < 3)
		return;
	for (int i=0; i<n; ++i) {
		float angle = getAngle(polygon[i], polygon[(i+1)%n], polygon[(i+2)%n]);
		if (angle < smallest && polygon[i].y >= polygon[(i+1)%n].y && polygon[(i+1)%n].y <= polygon[(i+2)%n].y) {
			smallest = angle;
			stippest = polygon[(i+1)%n];
		}
	}
}

void Detector::findBookRegion(vector<Point> cHull, Point& leftTop, Point& rightTop) {
	vector<Point>::iterator d = cHull.begin();
	Point leftP1(99999, 0);
	Point leftP2(99999, 0);
	Point rightP1(0, 0);
	Point rightP2(0, 0);
	while(d != cHull.end()) {
		Point p = *d++;
		if (leftP1.x > p.x) {
			leftP2 = leftP1;
			leftP1 = p;
		}
		else if(leftP2.x > p.x)
			leftP2 = p;
		if (rightP1.x < p.x) {
			rightP2 = rightP1;
			rightP1 = p;
		}
		else if(rightP2.x < p.x)
			rightP2 = p;
	}

	leftTop = leftP1.y<leftP2.y? leftP1: leftP2;
	rightTop = rightP1.y<rightP2.y? rightP1: rightP2;
}

void Detector::filter(Mat src, Mat& dst) {
	Mat tmp;
	blur(src, tmp, Size(3,3));
	tmp.convertTo(tmp, CV_8UC3, 2, 0);
	pyrDown(tmp, tmp);

	Mat binary, binary2;
	cvtColor(tmp, tmp, CV_BGR2HLS);
	inRange(tmp, Scalar(-72,-192,-74), Scalar(207,206,400), binary);
	inRange(tmp, Scalar(71,33,-154), Scalar(214,246,339), binary2);
	binary += binary2;
	medianBlur(binary, binary, 7);
	pyrUp(binary, dst);
}

void Detector::rotate(Mat src, Mat& dst, float angle) {
	Size s = src.size();
	Mat rot_mat = getRotationMatrix2D(Point(s)/2., angle, 1.0);
	warpAffine(src, dst, rot_mat, s);
}

int Detector::getPageNum(Mat src) {
	Mat pageImg;
	/* Right page number */
	// if (src.size().height > 30 && src.size().width > 30)
	// 	pageImg = src(Rect(20, src.size().height-30, 50, 30));

	/* Left page number */
	if (src.size().height > 30 && src.size().width > 30)
		pageImg = src(Rect(src.size().width-50, src.size().height-50, 50, 50));
	else
		pageImg = src.clone();

	Mat sharpen;
	pyrUp(pageImg, pageImg);
	GaussianBlur(pageImg, sharpen, Size(0,0), 3);
	addWeighted(pageImg, 1.5, sharpen, -0.5, 0, sharpen);
	cvtColor(sharpen, pageImg, CV_BGR2GRAY);
	imshow("6. Get page image", pageImg);

	api->SetImage((uchar*)pageImg.data, pageImg.cols, pageImg.rows, 1, pageImg.cols);
	char *outText = api->GetUTF8Text();

	if (strlen(outText) > 0)
		cout << outText << endl;
	return 0;
}

Point Detector::rel2abs(Point p) {
	Point res;
	float cosv = cos(PI*angle/180);
	float sinv = sin(PI*angle/180);
	res.x = (int)(cosv * p.x - sinv * p.y);
	res.y = (int)(sinv * p.x + cosv * p.y);
	return (res + bookCoord);
}

Point Detector::abs2rel(Point p) {
	Point res = p - bookCoord;
	float cosv = cos(PI*angle/180);
	float sinv = sin(PI*angle/180);
	res.x = (int)(  cosv  * p.x + sinv * p.y);
	res.y = (int)(-(sinv) * p.x + cosv * p.y);
	return res;
}

void Detector::cropBook(Mat src, Mat& bookImg, float angle, Mat mask) {
	Mat canvas = Mat(src.size(), CV_8UC3, Scalar(255,255,255));
	src.copyTo(canvas, mask);
	Mat bookArea;
	rotate(canvas, bookImg, angle);
	rotate(mask, bookArea, angle);
	bookImg = bookImg(boundingRect(bookArea));	
}

void Detector::detect(Mat frame, Mat& bookImg, int& pageNum, Point& relpoint) {
	Mat bookMask;
	detectBook(frame, bookCoord, angle, bookMask);
	cropBook(frame, bookImg, angle, bookMask);
	relpoint = detectTip(bookImg);
	pageNum = getPageNum(bookImg);
}

void Detector::detectBook(Mat src, Point &leftTop, float& angle, Mat& bookArea) {
	Mat binary, _tmp;
	filter(src, binary);
	bitwise_not(binary, _tmp);
	vector<vector<Point> > contours;
	findContours(_tmp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);

		Point rightTop;
		findBookRegion(cHull[i], leftTop, rightTop);
		angle = getAngle(rightTop, leftTop, Point(rightTop.x, leftTop.y));
		if (leftTop.y > rightTop.y)
			angle = -angle;

		bookArea = Mat(binary.size(), CV_8U, Scalar::all(0));
		fillConvexPoly(bookArea, &cHull[i][0], cHull[i].size(), 255, 8, 0);
	}
}

Point Detector::detectTip(Mat src) {
	vector<vector<Point> > contours;
	Mat binary;
	filter(src, binary);
	findContours(binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Point stippest = Point(-1, -1);
	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);
		findStippest(cHull[i], stippest);
	}
	return stippest;
}