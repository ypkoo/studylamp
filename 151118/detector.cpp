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

float Detector::distanceP2P(Point a, Point b) {
	return (float)sqrt(pow(a.x-b.x,2) + pow(a.y-b.y,2));
}

void Detector::rotate(Mat src, Point cen_pt, float width, float height, double angle, Mat& dst)
{
    Mat r = getRotationMatrix2D(cen_pt, angle, 1.0);
    Mat dst_temp;
    warpAffine(src, dst_temp, r, Size(width*abs(cos(angle))+height*abs(sin(angle)),width*abs(sin(angle))+height*abs(cos(angle))));
    if(MIN(width, dst_temp.size().width-cen_pt.x) > 0 && MIN(height, dst_temp.size().height-cen_pt.y) > 0)
	    dst = dst_temp(Rect(cen_pt.x, cen_pt.y, MIN(width, dst_temp.size().width-cen_pt.x), MIN(height, dst_temp.size().height-cen_pt.y))).clone();
	else dst = dst_temp.clone();
}

void Detector::findBookRegion(vector<Point> cHull, vector<Point>& endPoints) {
	vector<Point>::iterator d = cHull.begin();
	Point leftP_1(99999, 0);
	Point leftP_2(99999, 0);
	Point rightP_1(0, 0);
	Point rightP_2(0, 0);
	while(d != cHull.end()) {
		Point p = *d++;
		if (leftP_1.x > p.x) {
			leftP_2 = leftP_1;
			leftP_1 = p;
		}
		else if(leftP_2.x > p.x)
			leftP_2 = p;
		if (rightP_1.x < p.x) {
			rightP_2 = rightP_1;
			rightP_1 = p;
		}
		else if(rightP_2.x < p.x)
			rightP_2 = p;
	}

	Point leftTopP, leftBotP, rightTopP, rightBotP;
	if (leftP_1.y < leftP_2.y) {
		leftTopP = leftP_1;
		leftBotP = leftP_2;
	}
	else {
		leftTopP = leftP_2;
		leftBotP = leftP_1;
	}
	if (rightP_1.y < rightP_2.y) {
		rightTopP = rightP_1;
		rightBotP = rightP_2;
	}
	else {
		rightTopP = rightP_2;
		rightBotP = rightP_1;
	}
	endPoints.push_back(leftTopP);
	endPoints.push_back(leftBotP);
	endPoints.push_back(rightTopP);
	endPoints.push_back(rightBotP);
}

void Detector::detectBook(Mat frame, Mat& bookImg, Mat& pageImg, Mat& rot_mat, Point& leftTopPoint) {
	Mat reduced, contrast, _contrast;
	pyrDown(frame, reduced);
	blur(reduced, contrast, Size(3,3));
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

		vector<Point> rectP;
		findBookRegion(cHull[i], rectP);
		float angle = getAngle(rectP[2], rectP[0], Point(rectP[2].x, rectP[0].y));
		if (rectP[0].y > rectP[2].y)
			angle = -angle;

		Point2f frameSize(reduced.size());
		Mat mask(Size(frameSize), CV_8U, Scalar::all(0));
		fillConvexPoly(mask, &cHull[i][0], cHull[i].size(), 255, 8, 0);
		bookImg = Mat(Size(frameSize), CV_8UC3, Scalar(214,186,149));
		reduced.copyTo(bookImg, mask);
		rot_mat = getRotationMatrix2D(frameSize/2, angle, 1.0);
		leftTopPoint = rectP[0];
		Mat rotatedFrame, rotatedMask;
		warpAffine(bookImg, rotatedFrame, rot_mat, Size(frameSize));
		warpAffine(mask, rotatedMask, rot_mat, Size(frameSize));
		Mat rotatedSrc = rotatedFrame(boundingRect(Mat(rotatedMask)));
		imshow("book", rotatedSrc);

		bookImg = rotatedSrc.clone();

		if (rotatedSrc.size().height > 30 && rotatedSrc.size().width > 30) {
			pageImg = rotatedSrc(Rect(20, rotatedSrc.size().height-30, 50, 30));
			Mat sharpen;
			pyrUp(pageImg, pageImg);
			GaussianBlur(pageImg, sharpen, Size(0,0), 3);
			addWeighted(pageImg, 1.5, sharpen, -0.5, 0, sharpen);
			cvtColor(sharpen, pageImg, CV_BGR2GRAY);
		}
		else pageImg = rotatedSrc.clone();
	}
#ifdef DEBUG
	Mat showFrame;
	rectangle(reduced, boundingRect(Mat(contours[i])), Scalar(244,244,244),3,8,0);
	showFrame = reduced.clone();
	pyrDown(binary, binary);
	pyrDown(binary, binary);
	vector<Mat> channels;
	Mat result;
	for(int i=0; i<3; ++i)
		channels.push_back(binary);
	merge(channels, result);
	Rect roi(Point(0, 0), binary.size());
	result.copyTo(showFrame(roi));
	imshow("Detecting Book", showFrame);
	imshow("Page number image", pageImg);
#endif
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

	return stippest*2;
}