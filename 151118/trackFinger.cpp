#include "trackFinger.hpp"
#include "main.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

TrackFinger::TrackFinger() {
	c_lower[0] = 9;
	c_upper[0] = 255;
	c_lower[1] = 104;
	c_upper[1] = 255;
	c_lower[2] = 3;
	c_upper[2] = 255;

#ifdef DEBUG
	    namedWindow("trackbars", CV_WINDOW_AUTOSIZE);
	    resizeWindow("trackbars", 1004, 50);
		createTrackbar("S_lower", "trackbars", &c_lower[1],255);
		createTrackbar("L_lower", "trackbars", &c_lower[2],255);
		createTrackbar("H_upper", "trackbars", &c_upper[0],255);
	  	createTrackbar("S_upper", "trackbars", &c_upper[1],255);
		createTrackbar("H_lower", "trackbars", &c_lower[0],255);
		createTrackbar("L_upper", "trackbars", &c_upper[2],255);
#endif
}

void TrackFinger::produceBinaries(Mat original, Mat& binary) {
	Scalar lowerBound = Scalar(c_lower[0] , c_lower[1], c_lower[2]);
	Scalar upperBound = Scalar(c_upper[0] , c_upper[1], c_upper[2]);

	pyrDown(original, binary);
	blur(binary, binary, Size(3,3));
	cvtColor(binary, binary, CV_BGR2HLS);
	inRange(binary, lowerBound, upperBound, binary);
	medianBlur(binary, binary, 7);
	pyrUp(binary, binary);
}

int TrackFinger::findBiggestContour(vector<vector<Point> > contours) {
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

float TrackFinger::getAngle(Point s, Point f, Point e) {
	float l1 = sqrt(pow(f.x-s.x,2) + pow(f.y-s.y,2));
	float l2 = sqrt(pow(f.x-e.x,2) + pow(f.y-e.y,2));
	float dot = (s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	float angle = acos(dot/(l1*l2));
	angle = angle * 180/PI;
	return angle;
}

void TrackFinger::findStippest(vector<Point> cHull, Point& stippest) {
	int n = cHull.size();
	float smallest = 360;
	if (n >= 3)
		for (int i=0; i<n; ++i) {
			float angle = getAngle(cHull[i], cHull[(i+1)%n], cHull[(i+2)%n]);
			if (angle < smallest) {
				smallest = angle;
				stippest = cHull[(i+1)%n];
			}
		}
}

Point TrackFinger::getFingerPoint(Mat frame) {
	Mat original, binary;
	pyrDown(frame, original);
	produceBinaries(original, binary);

	vector<vector<Point> > contours;
	Mat _binary = binary.clone();
	findContours(_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int i = findBiggestContour(contours);
	vector<vector<Point> > cHull = vector<vector<Point> >(contours.size());

	Point stippest = Point(-1, -1);
	if(i != -1) {
		convexHull(Mat(contours[i]), cHull[i], false, true);
		approxPolyDP(Mat(cHull[i]), cHull[i], 15, true);

		findStippest(cHull[i], stippest);
	
#ifdef DEBUG
		if (stippest != Point(-1, -1)) {
			drawContours(original, cHull, i, Scalar(0,0,200), 2, 8, vector<Vec4i>(), 0, Point());
	   		circle(original, stippest, 4, Scalar(200,255,3), 6);
		}
		pyrDown(binary, binary);
		pyrDown(binary, binary);
		vector<Mat> channels;
		Mat result;
		for(int i=0;i<3;i++)
			channels.push_back(binary);
		merge(channels, result);
		Rect roi(Point(0, 0), binary.size());
		result.copyTo(original(roi));
		imshow("TrackingFingerResult", original);
#endif
	}

	return stippest * 2;
}