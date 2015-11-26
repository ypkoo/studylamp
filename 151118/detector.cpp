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

using namespace cv;
using namespace std;

Detector::Detector() {}

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
	float dot = (s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	return acos(dot/(l1*l2));
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

Point Detector::RotatePoint(const Point2f& p, float rad) {
    const float x = cos(rad) * p.x - sin(rad) * p.y;
    const float y = sin(rad) * p.x + cos(rad) * p.y;
    Point rot_p(x, y);
    if (x < 0) rot_p.x = 0;
    if (y < 0) rot_p.y = 0;
    return rot_p;
}

Point Detector::RotatePoint(Point cen_pt, Point p, float rad) {
    Point trans_pt = p - cen_pt;
    Point rot_pt   = RotatePoint(trans_pt, rad);
    Point fin_pt   = rot_pt + cen_pt;
    return fin_pt;
}

void Detector::rotate(Mat src, double angle, Mat& dst) {
    int len = max(src.cols, src.rows);
    Point pt(src.size().width/2., src.size().height/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);

    warpAffine(src, dst, r, src.size());
}


void Detector::detectBook(Mat frame, Mat& bookImg, Mat& pageImg) {
	Mat reduced, contrast, _contrast;
	pyrDown(frame, reduced);
	reduced.convertTo(contrast, -1, 2, 0);
	pyrDown(contrast, _contrast);

	Mat binary, binary2;
	cvtColor(_contrast, _contrast, CV_BGR2HLS);
	inRange(_contrast, Scalar(-72,-192,-74), Scalar(207,206,400), binary);
	inRange(_contrast, Scalar(71,33,-154), Scalar(214,246,339), binary2);
	binary += binary2;
	medianBlur(binary, binary, 7);
	bitwise_not(binary, binary);
	pyrUp(binary, binary);
	
	vector<vector<Point> > contours;
	Mat _binary = binary.clone();
	findContours(_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);

		//Rect bRect = boundingRect(Mat(contours[i]));
		Mat mask(reduced.size(), CV_8U, Scalar::all(0));
		//mask(Rect(bRect.x+5, bRect.y+5, bRect.width-10, bRect.height-10)) = 1;
		fillConvexPoly(mask, &cHull[i][0], cHull[i].size(), 255, 8, 0);
		bookImg = Mat(reduced.size(), CV_8UC3, Scalar(214,186,149));
		reduced.copyTo(bookImg, mask);
		//bookImg = bookImg(Rect(bRect.x+5, bRect.y+5, bRect.width-10, bRect.height-10));

		vector<Point>::iterator d = cHull[i].begin();
		Point leftP_1, leftP_2, rightP_1, rightP_2;
		leftP_1.x = contrast.size().width;
		leftP_2.x = contrast.size().width;
		while(d != cHull[i].end()){
			if (leftP_1.x > (*d).x)
			{
				leftP_2 = leftP_1;
				leftP_1 = (*d);
			}else if(leftP_2.x > (*d).x){
				leftP_2 = (*d);
			}else{
				
			}
			if (rightP_1.x < (*d).x)
			{
				rightP_2 = rightP_1;
				rightP_1 = (*d);
			}else if(rightP_2.x < (*d).x){
				rightP_2 = (*d);
			}

			(*d++);
		}
		Point leftTopP, leftBotP, rightTopP, rightBotP;
		if (leftP_1.y < leftP_2.y)
		{
			leftTopP = leftP_1;
			leftBotP = leftP_2;
		}else{
			leftTopP = leftP_2;
			leftBotP = leftP_1;
		}
		if (rightP_1.y < rightP_2.y)
		{
			rightTopP = rightP_1;
			rightBotP = rightP_2;
		}else{
			rightTopP = rightP_2;
			rightBotP = rightP_1;
		}
		float angle = getAngle(rightBotP, leftBotP, Point(rightBotP.x, leftBotP.y));
		if (leftBotP.y < rightBotP.y) angle = -angle;

		Mat rotated;

		rotate(frame, -angle, rotated);
		Point rotatedRightBotP = RotatePoint(Point(contrast.size().width/2., contrast.size().height/2.), rightBotP, angle);
		Point rotatedLeftBotP = RotatePoint(Point(contrast.size().width/2., contrast.size().height/2.), leftBotP, angle);
		//rotatedRightBotP = rotatedRightBotP * 2;
		//rotatedLeftBotP = rotatedLeftBotP * 2;
		pageImg = rotated(Rect(rotatedRightBotP.x-80, rotatedRightBotP.y-50, 80, 50)).clone();

		Mat sharpen;
		pyrUp(pageImg, pageImg);
		GaussianBlur(pageImg, sharpen, Size(0, 0), 3);
		addWeighted(pageImg, 1.5, sharpen, -0.5, 0, sharpen);
		cvtColor(sharpen, pageImg, CV_BGR2GRAY);
#ifdef DEBUG
		imshow("Book image", bookImg);
		imshow("Page number image", pageImg);
#endif
	}
}

Point Detector::detectTip(Mat frame) {
	Mat binary;
	blur(frame, binary, Size(3,3));
	cvtColor(binary, binary, CV_BGR2HLS);
	inRange(binary, Scalar(9, 104, 3), Scalar(255, 255, 255), binary);
	medianBlur(binary, binary, 3);
	bitwise_not(binary, binary);

	vector<vector<Point> > contours;
	Mat _binary = binary.clone();
	findContours(_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Point stippest = Point(-1, -1);

	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);
		findStippest(cHull[i], stippest);
	
#ifdef DEBUG
		Mat showFrame = frame.clone();
		if (stippest != Point(-1, -1)) {
			drawContours(showFrame, cHull, i, Scalar(0,0,200), 2, 8, vector<Vec4i>(), 0, Point());
	   		circle(showFrame, stippest, 4, Scalar(200,255,3), 6);
		}
		Mat mask(showFrame.size(), CV_8U, Scalar::all(0));
		fillConvexPoly(mask, &cHull[i][0], cHull[i].size(), 255, 8, 0);
		pyrDown(binary, binary);
		pyrDown(binary, binary);
		vector<Mat> channels;
		Mat result;
		for(int i=0; i<3; ++i)
			channels.push_back(binary);
		merge(channels, result);
		Rect roi(Point(0, 0), binary.size());
		result.copyTo(showFrame(roi));
		imshow("Detecting Tip", showFrame);
#endif
	}

	return stippest;
}