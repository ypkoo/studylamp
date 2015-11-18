#include <windows.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "myImage.hpp"
#include <vector>
#include <cmath>
// #include <sys/time.h>
#include "main.hpp"
#include "gesture.hpp"

using namespace cv;
using namespace std;

/* Global Variables  */

int fontFace = FONT_HERSHEY_PLAIN;
int c_lower[3];
int c_upper[3];
Rect bRect;

/* end global variables */


void initWindows(MyImage m){
    namedWindow("img1",CV_WINDOW_AUTOSIZE);
    namedWindow("trackbars",CV_WINDOW_NORMAL);
    resizeWindow("trackbars", 512, 50);
}

void initTrackbars(){
	c_lower[0]=9;
	c_upper[0]=255;
	c_lower[1]=104;
	c_upper[1]=255;
	c_lower[2]=3;
	c_upper[2]=255;
	c_lower[0]=0;
	c_upper[0]=32;
	c_lower[1]=120;
	c_upper[1]=255;
	c_lower[2]=30;
	c_upper[2]=170;
	createTrackbar("S_lower","trackbars",&c_lower[1],255);
	createTrackbar("L_lower","trackbars",&c_lower[2],255);
	createTrackbar("H_upper","trackbars",&c_upper[0],255);
  	createTrackbar("S_upper","trackbars",&c_upper[1],255);
	createTrackbar("H_lower","trackbars",&c_lower[0],255);
	createTrackbar("L_upper","trackbars",&c_upper[2],255);
}

void produceBinaries(MyImage *m){
	Scalar lowerBound = Scalar(c_lower[0] , c_lower[1], c_lower[2]);
	Scalar upperBound = Scalar(c_upper[0] , c_upper[1], c_upper[2]);

	inRange(m->srcLR, lowerBound, upperBound, m->bw);
	medianBlur(m->bw, m->bw, 7);
}

int findBiggestContour(vector<vector<Point> > contours){
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

float getAngle(Point s, Point f, Point e){
	float l1 = sqrt(pow(f.x-s.x,2) + pow(f.y-s.y,2));
	float l2 = sqrt(pow(f.x-e.x,2) + pow(f.y-e.y,2));
	float dot=(s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
	float angle = acos(dot/(l1*l2));
	angle = angle * 180/PI;
	return angle;
}

void makeContours(gesture &gest, MyImage *m, unsigned int tick){
	Mat aBw;
	pyrUp(m->bw,m->bw);
	//bitwise_not(m->bw, m->bw);
	m->bw.copyTo(aBw);
	vector<vector<Point> > contours;
	vector<vector<Point> >hullP;
	findContours(aBw,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	hullP=vector<vector<Point> >(contours.size());
	int cIdx = findBiggestContour(contours);
	if(cIdx!=-1){
		Mat biggest = Mat(contours[cIdx]);
		bRect=boundingRect(biggest);
		convexHull(biggest, hullP[cIdx], false, true);
		approxPolyDP(Mat(hullP[cIdx]), hullP[cIdx], 15, true);

		vector<Point>::iterator d = hullP[cIdx].begin();
		int c = 0;
		Point x[100];
	    while(d != hullP[cIdx].end()) {
	   	    Point next = (*d);
			x[c++] = (*d++);
		}
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
			putText(m->src, "Stippest", stippest-Point(0,20), fontFace, 1.2f, Scalar(200,255,30), 2);
			rectangle(m->src,bRect.tl(),bRect.br(),Scalar(200,30,200));
			drawContours(m->src, hullP, cIdx, Scalar(0,0,200), 2, 8, vector<Vec4i>(), 0, Point());
	   		circle(m->src, stippest, 4, Scalar(200,255,3), 6);
			gest.registerPoint(stippest.x, stippest.y, tick);
   		}
	}
}

void showWindows(MyImage m){
	pyrDown(m.bw,m.bw);
	pyrDown(m.bw,m.bw);
	Rect roi( Point( 3*m.src.cols/4,0 ), m.bw.size());
	vector<Mat> channels;
	Mat result;
	for(int i=0;i<3;i++)
		channels.push_back(m.bw);
	merge(channels, result);
	result.copyTo(m.src(roi));
	imshow("img1", m.src);	
}

int main(){
	MyImage m(0);

	gesture gest;

	initWindows(m);
	initTrackbars();
	for(;;){

		m.cap >> m.src;
		flip(m.src, m.src, 1);
		pyrDown(m.src, m.src);
    	pyrDown(m.src, m.srcLR);
		blur(m.srcLR, m.srcLR, Size(3,3));
		cvtColor(m.srcLR,m.srcLR,ORIGCOL2COL);
		produceBinaries(&m);
		cvtColor(m.srcLR,m.srcLR,COL2ORIGCOL);
		makeContours(gest, &m, GetTickCount());
		gest.visualize(m.src); 
		showWindows(m);
  		if(cv::waitKey(30) == char('q')) break;
	}
	destroyAllWindows();
	m.cap.release();
	return 0;
}