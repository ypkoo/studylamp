#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "detectBook.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;
using namespace tesseract;

tesseract::TessBaseAPI *api;

DetectBook::DetectBook() {
	pageNum = -1;

	api = new tesseract::TessBaseAPI();
	api->SetPageSegMode(PSM_SINGLE_BLOCK);
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
}

void DetectBook::produceBinaries() {
	Scalar lowerBound;
	Scalar upperBound;
	for(int i=0;i<2;i++){
		if (i==0){
			lowerBound=Scalar( -72, -192, -74 );
			upperBound=Scalar( 207, 206, 400 );
		}else{
			lowerBound=Scalar( 71, 33, -154 );
			upperBound=Scalar( 214, 246, 339 );
		}
		binaryList.push_back(Mat(contrastLRLRLR.rows,contrastLRLRLR.cols,CV_8U));	
		inRange(contrastLRLRLR,lowerBound,upperBound,binaryList[i]);	
	}
	binaryList[0].copyTo(binary);
	for(int i=1;i<2;i++){
		binary+=binaryList[i];	
	}
	binary = 255-binary;
	medianBlur(binary, binary,7);
}

float DetectBook::distanceP2P(Point a, Point b){
	float d= sqrt(fabs( pow(a.x-b.x,2) + pow(a.y-b.y,2) )) ;  
	return d;
}

Point DetectBook::RotatePoint(const Point2f& p, float rad)
{
    const float x = cos(rad) * p.x - sin(rad) * p.y;
    const float y = sin(rad) * p.x + cos(rad) * p.y;

    Point rot_p(x, y);
    return rot_p;
}

Point DetectBook::RotatePoint(Point cen_pt, Point p, float rad)
{
    Point trans_pt = p - cen_pt;
    Point rot_pt   = RotatePoint(trans_pt, rad);
    Point fin_pt   = rot_pt + cen_pt;

    return fin_pt;
}

void DetectBook::rotate(Mat src, double angle, Mat& dst)
{
    int len = max(src.cols, src.rows);
    Point pt(src.size().width/2., src.size().height/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);

    warpAffine(src, dst, r, src.size());
}

void DetectBook::setImage(Mat img){
	img.copyTo(original);
	pyrDown(original, originalLRLR);
	pyrDown(originalLRLR, originalLRLR);
	originalLRLR.convertTo(contrastLRLR, -1, 2, 0);
	pyrDown(contrastLRLR, contrastLRLRLR);
}

Mat DetectBook::cropBook(){
	produceBinaries();
	
	Mat aBw;
	pyrUp(binary,binary);
	binary.copyTo(aBw);

	vector<vector<Point> > contours;
	findContours(aBw,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	hullP = vector<vector<Point> >(contours.size());
	cIdx = findBiggestContour(contours);

	Mat imgTemp;
	imgTemp = Mat(contrastLRLR.size(), CV_8UC3, Scalar(214,186,149));
	if(cIdx!=-1){
		bRect=boundingRect(Mat(contours[cIdx]));
		convexHull(Mat(contours[cIdx]),hullP[cIdx],false,true);
		approxPolyDP( Mat(hullP[cIdx]),hullP[cIdx], 18, true );

	
		//m->srcOrig.copyTo(m->srcCrop, m->bw);
		Mat mask(contrastLRLR.size(),CV_8U,Scalar::all(0));
		mask(Rect(bRect.x+5, bRect.y+5, bRect.width-10, bRect.height-10)) = 1;
		originalLRLR.copyTo(imgTemp, mask);
	}

	return imgTemp;
}

int DetectBook::getPageNum(){

	vector<Point>::iterator d = hullP[cIdx].begin();
	Point leftP_1, leftP_2, rightP_1, rightP_2;
	leftP_1.x = contrastLRLR.size().width;
	leftP_2.x = contrastLRLR.size().width;
	while(d != hullP[cIdx].end()){
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
	cout << "leftTopP " << leftTopP <<" leftBotP "<< leftBotP << " rightTopP " << rightTopP << " rightBotP " << rightBotP << endl;
	float angle = acos((float)(rightBotP.x-leftBotP.x)/(float)distanceP2P(leftBotP, rightBotP));
	if (leftBotP.y < rightBotP.y)
	{
		angle = -angle;
	}

	// cout << "angle: " << angle*180/PI << endl;
	Mat rotatedSrc;
	rotate(original, -angle*180/PI, rotatedSrc);
	Point rotatedRightBotP = RotatePoint(Point(contrastLRLR.size().width/2., contrastLRLR.size().height/2.), rightBotP, angle);
	Point rotatedLeftBotP = RotatePoint(Point(contrastLRLR.size().width/2., contrastLRLR.size().height/2.), leftBotP, angle);
	rotatedRightBotP = rotatedRightBotP*4;
	rotatedLeftBotP = rotatedLeftBotP*4;
	Mat pageNumR = rotatedSrc(Rect(rotatedRightBotP.x-80, rotatedRightBotP.y-50, 80, 70)).clone();
	Mat pageNumL = rotatedSrc(Rect(rotatedLeftBotP.x, rotatedLeftBotP.y-50, 100, 70)).clone();

	// imshow("img3", pageNumL);
	char *outText;
	Mat sharpen, gray;
	pyrUp(pageNumR, pageNumR);
	GaussianBlur(pageNumR, sharpen, Size(0, 0), 3);
	addWeighted(pageNumR, 1.5, sharpen, -0.5, 0, sharpen);

	cvtColor(sharpen, gray, CV_BGR2GRAY);
	
	api->SetImage((uchar*)gray.data, gray.cols, gray.rows, 1, gray.cols);
	outText = api->GetUTF8Text();
	
	int tempPageNum = atoi(outText);
	if (tempPageNum>0)
	{
		pageNum = tempPageNum;
	}

	printf("OCR output:%s\n", outText);
	cout << "page number is: " << atoi(outText) << endl;
	delete [] outText;

	return pageNum;
	// imshow("img2", gray);
}