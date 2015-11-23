#include "trackFinger.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui_c.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

TrackFinger::TrackFinger(bool _debug) {
	debug = _debug;
	if (debug) 
    	namedWindow("img1", CV_WINDOW_AUTOSIZE);
}

void TrackFinger::initThreshold() {
	c_lower[0] = 9;
	c_upper[0] = 255;
	c_lower[1] = 104;
	c_upper[1] = 255;
	c_lower[2] = 3;
	c_upper[2] = 255;
	if (debug) {
	    namedWindow("trackbars", CV_WINDOW_AUTOSIZE);
	    resizeWindow("trackbars", 512, 50);
		createTrackbar("S_lower", "trackbars", &c_lower[1],255);
		createTrackbar("L_lower", "trackbars", &c_lower[2],255);
		createTrackbar("H_upper", "trackbars", &c_upper[0],255);
	  	createTrackbar("S_upper", "trackbars", &c_upper[1],255);
		createTrackbar("H_lower", "trackbars", &c_lower[0],255);
		createTrackbar("L_upper", "trackbars", &c_upper[2],255);
	}
}

void TrackFinger::produceBinaries() {
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

Point TrackFinger::getFingerPoint(Mat img) {
	flip(img, original, 1);
	pyrDown(original, original);
	produceBinaries();

	vector<vector<Point> > contours;
	Mat _binary;
	binary.copyTo(_binary);
	findContours(_binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<vector<Point> > hullP = vector<vector<Point> >(contours.size());
	int cIdx = findBiggestContour(contours);

	if(cIdx != -1) {
		Mat biggest = Mat(contours[cIdx]);
		convexHull(biggest, hullP[cIdx], false, true);
		approxPolyDP(Mat(hullP[cIdx]), hullP[cIdx], 15, true);

		vector<Point>::iterator d = hullP[cIdx].begin();
		int c = 0;
		Point x[100];
	    while(d != hullP[cIdx].end())
			x[c++] = (*d++);
		
		if (c >= 3) {
			int sAngle = 360;
			Point stippest;
			for (int i=0; i<c; ++i) {
				int angle = (int)getAngle(x[i], x[(i+1)%c], x[(i+2)%c]);
				if (angle < sAngle) {
					sAngle = angle;
					stippest = x[(i+1)%c];
				}
			}
			
			if (debug)
			{
				putText(original, "Stippest", stippest-Point(0,20), fontFace, 1.2f, Scalar(200,255,30), 2);
				bRect = boundingRect(biggest);
				rectangle(original, bRect.tl(), bRect.br(), Scalar(200,30,200));
				drawContours(original, hullP, cIdx, Scalar(0,0,200), 2, 8, vector<Vec4i>(), 0, Point());
		   		circle(original, stippest, 4, Scalar(200,255,3), 6);
		   		visualize();
		   	}
	   		
	   		return stippest;
   		}
	}

	return Point(-1, -1);
}

void TrackFinger::visualize() {
	pyrDown(binary, binary);
	pyrDown(binary, binary);
	Rect roi(Point(3*original.cols/4, 0), binary.size());
	vector<Mat> channels;
	Mat result;
	for(int i=0;i<3;i++)
		channels.push_back(binary);
	merge(channels, result);
	result.copyTo(original(roi));
	imshow("img1", original);	
}
