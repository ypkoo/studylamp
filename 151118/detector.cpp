#include "detector.hpp"
#include "main.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

Detector::Detector() {}

void Detector::produceBinaries(Mat src, Mat& dst, Scalar lowerBound, Scalar upperBound) {
	pyrDown(src, dst);
	blur(dst, dst, Size(3,3));
	cvtColor(dst, dst, CV_BGR2HLS);
	inRange(dst, lowerBound, upperBound, dst);
	medianBlur(dst, dst, 7);
	bitwise_not(dst, dst);
	pyrUp(dst, dst);
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
	float dot = (s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	float angle = acos(dot/(l1*l2));
	angle = angle * 180/PI;
	return angle;
}

void Detector::findStippest(vector<Point> polygon, Point& stippest) {
	int n = polygon.size();
	float smallest = 360;
	if (n < 3)
		return;
	for (int i=0; i<n; ++i) {
		float angle = getAngle(polygon[i], polygon[(i+1)%n], polygon[(i+2)%n]);
		if (angle < smallest) {
			smallest = angle;
			stippest = polygon[(i+1)%n];
		}
	}
}

void Detector::detectBook(Mat frame, Mat& dst, int& pageNum){
	Mat reduced, contrast, _contrast;
	pyrDown(frame, reduced);
	reduced.convertTo(contrast, -1, 2, 0);
	pyrDown(contrast, _contrast);
	Mat binary, binary2;
	//inRange(_contrast, Scalar(9, 104, 3), Scalar(255, 255, 255), binary);
	inRange(_contrast, Scalar(-72,-192,-74), Scalar(207,206,400), binary);
	inRange(_contrast, Scalar(71,33,-154), Scalar(214,246,339), binary2);
	binary += binary2;
	medianBlur(binary, binary, 7);
	bitwise_not(binary, binary);
	pyrUp(binary, binary);

	dst = Mat(reduced.size(), CV_8UC3, Scalar(214,186,149));
	
	vector<vector<Point> > contours;
	Mat _binary = binary.clone();
	findContours(_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);

		Mat mask(reduced.size(), CV_8U, Scalar::all(0));
		fillConvexPoly(mask, &cHull[i][0], cHull[i].size(), 255, 8, 0);
		//Rect bRect = boundingRect(Mat(contours[i]));
		//mask(Rect(bRect.x+5, bRect.y+5, bRect.width-10, bRect.height-10)) = 1;
		reduced.copyTo(dst, mask);
	}
}

Point Detector::detectTip(Mat frame) {
	Mat reduced, binary;
	pyrDown(frame, reduced);
	blur(frame, binary, Size(3,3));
	cvtColor(binary, binary, CV_BGR2HLS);
	inRange(binary, Scalar(9, 104, 3), Scalar(255, 255, 255), binary);
	medianBlur(binary, binary, 3);
	bitwise_not(binary, binary);
	//pyrUp(binary, binary);

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

	Mat mask(frame.size(), CV_8U, Scalar::all(0));
	fillConvexPoly(mask, &cHull[i][0], cHull[i].size(), 255, 8, 0);
	
#ifdef DEBUG
		if (stippest != Point(-1, -1)) {
			drawContours(frame, cHull, i, Scalar(0,0,200), 2, 8, vector<Vec4i>(), 0, Point());
	   		circle(frame, stippest, 4, Scalar(200,255,3), 6);
		}
		pyrDown(binary, binary);
		pyrDown(binary, binary);
		vector<Mat> channels;
		Mat result;
		for(int i=0;i<3;i++)
			channels.push_back(binary);
		merge(channels, result);
		Rect roi(Point(0, 0), binary.size());
		result.copyTo(frame(roi));
		imshow("img2", frame);
#endif
	}

	return stippest;
}