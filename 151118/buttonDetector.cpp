
#include "common.hpp"
#include "buttonDetector.hpp"

#define PI 3.14159
#define NUMBER_OF_BUTTONS 3
// #define BUTTON_PRESS_TICK 500 //in ms
// #define BUTTON_PRESS_INTER_TICK 1500 //in ms

ButtonDetector::ButtonDetector(unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;
	projRect = Rect(950*width/2592,270*height/1944,720*width/2592,460*height/1944);
	status = STATUS_BOOKCOVER; // 0
	init();
}
void ButtonDetector::init(){

	for (int i = 0; i < NUMBER_OF_STATUS; ++i)
	{
		vector<Mat> buttonMaskVec;
		vector<unsigned int> thresholdVec;

		buttonMasks.push_back(buttonMaskVec);
		thresholds.push_back(thresholdVec);
	}

	for (int i = 0; i < NUMBER_OF_STATUS; ++i)
	{
		int bottomPadding = 150 * height/1944;
		int buttonRadius = 50 * width/2592; // or 50 * 1944 / height
		if (i==1) {

			Mat buttonMask0(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask0, Point(buttonMask0.size().width*8/32,buttonMask0.size().height-220*height/1944), 50*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask0);
			thresholds[i].push_back(1000);

			Mat buttonMask1(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask1, Point(buttonMask1.size().width*16/32,buttonMask1.size().height-220*height/1944), 50*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask1);
			thresholds[i].push_back(1000);

			Mat buttonMask2(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask2, Point(buttonMask2.size().width*24/32,buttonMask2.size().height-220*height/1944), 50*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask2);
			thresholds[i].push_back(1000);

			Mat shutDownButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(shutDownButtonMask, Point(shutDownButtonMask.size().width-40*width/2592,30*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(shutDownButtonMask);
			thresholds[i].push_back(500);
		}
		else if (i==2) {

			Mat videoOnOffMask(projRect.size(), CV_8U, Scalar::all(0));
			// rectangle(videoOnOffMask, Rect(150*width/2592, 150*height/1944, videoOnOffMask.size().width-150*2*width/2592, 200*height/1944), Scalar(255,255,255), -1);
			// rectangle(videoOnOffMask, Rect(200*width/2592, 150*height/1944, videoOnOffMask.size().width-200*2*width/2592, 150*height/1944), Scalar(0,0,0), -1);
			rectangle(videoOnOffMask, Rect(200*width/2592, 150*height/1944, videoOnOffMask.size().width-200*2*width/2592, 150*height/1944), Scalar(255,255,255), -1);
			buttonMasks[i].push_back(videoOnOffMask);
			thresholds[i].push_back(1000);

			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-45*width/2592,backButtonMask.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(500);  

		}
		else if (i==3) {
			Mat examineButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(examineButtonMask, Point(360*width/2592,230*height/1944), 60*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(examineButtonMask);
			thresholds[i].push_back(1000); 


			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-45*width/2592,backButtonMask.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(500); 
		}
		else if (i==4) {
			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-45*width/2592,backButtonMask.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(500);   
		}
		else if (i==5) {
			Mat backButtonMask0(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask0, Point(70*width/2592,backButtonMask0.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask0);
			thresholds[i].push_back(200);   

			Mat backButtonMask1(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask1, Point(150*width/2592,backButtonMask1.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask1);
			thresholds[i].push_back(200);   

			Mat backButtonMask2(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask2, Point(230*width/2592,backButtonMask2.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask2);
			thresholds[i].push_back(200);   

			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-45*width/2592,backButtonMask.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(500);   
		}
		else if (i==6) {
			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-45*width/2592,backButtonMask.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(500);   
		}
	}
}
void ButtonDetector::setStatus(enum PROGRAM_STATUS newStatus){
	status = newStatus;
}
void ButtonDetector::setInitFrame(Mat initFrame){
	initProjFrame = initFrame(projRect).clone();
}
vector<unsigned int> ButtonDetector::registerFrame(Mat newFrame){
	vector<unsigned int> pushedButtons;
	Mat projFrame = newFrame(projRect);
	
	circle(projFrame, Point(70*width/2592,projFrame.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), 2);
	circle(projFrame, Point(150*width/2592,projFrame.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), 2);
	circle(projFrame, Point(230*width/2592,projFrame.size().height-137*height/1944), 30*width/2592, Scalar(255,255,255), 2);
	

	imshow("projRegion", projFrame);

	Mat binary;
	// cout << initProjFrame.size() << endl;
	absdiff(initProjFrame, projFrame, binary);
	cvtColor(binary, binary, CV_BGR2GRAY);
	medianBlur(binary, binary, 9);
	inRange(binary, Scalar(30, 30, 30), Scalar(255, 255, 255), binary);
	medianBlur(binary, binary, 3);
	imshow("binary", binary);

	addWeighted(initProjFrame, 0.98, projFrame, 0.02, 0.0, initProjFrame);

	// unsigned int buttonWhites = 0;
	// unsigned int totalWhites = countNonZero(binary == 255);

	for (int i = 0; i < buttonMasks[status].size(); ++i)
	{
		Mat buttonDiff = binary & buttonMasks[status][i];
		int countWhite = countNonZero(buttonDiff == 255);
		// buttonWhites += countWhite;
		if (countWhite > thresholds[status][i])
		{
			// cout << "1";
			pushedButtons.push_back(1);
		}
		else
		{
			// cout << "0";
			pushedButtons.push_back(0);
		}
	}
	cout << endl;

	// cout << "totalWhites " << totalWhites << " buttonWhites " << buttonWhites;
	// if ((totalWhites > 1000*width*height/(2592*1944) && totalWhites > buttonWhites * 10) || totalWhites > 10000*width*height/(2592*1944) ){
		// cout << " reset frame";
		// setInitFrame(newFrame);
	// }

	// cout << endl;

	return pushedButtons;
}