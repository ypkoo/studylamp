#ifdef _WIN32
#include <windows.h>
#include "messenger.hpp"
#elif __APPLE__
#include <sys/time.h>
#endif

#include "buttonDetector.hpp"

#define PI 3.14159
#define NUMBER_OF_BUTTONS 3
#define NUMBER_OF_STATUS 7
#define BUTTON_PRESS_TICK 500 //in ms
#define BUTTON_PRESS_INTER_TICK 1500 //in ms

ButtonDetector::ButtonDetector()
{
	projRect = Rect(980,440,650,450);
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
		if (i==1)
		{

			int bottomPadding = 150;
			int buttonRadius = 50;

			Mat buttonMask0(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask0, Point(buttonMask0.size().width/4+20,buttonMask0.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask0);
			thresholds[i].push_back(1000);

			Mat buttonMask1(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask1, Point(buttonMask1.size().width/2+5,buttonMask1.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask1);
			thresholds[i].push_back(1000);

			Mat buttonMask2(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask2, Point(buttonMask2.size().width*3/4-15,buttonMask2.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask2);
			thresholds[i].push_back(1000);

			Mat buttonMask0Bot(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask0Bot, Point(buttonMask0Bot.size().width/4+20,buttonMask0Bot.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask0Bot);
			thresholds[i].push_back(1000);

			Mat buttonMask1Bot(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask1Bot, Point(buttonMask1Bot.size().width/2+5,buttonMask1Bot.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask1Bot);
			thresholds[i].push_back(1000);

			Mat buttonMask2Bot(projRect.size(), CV_8U, Scalar::all(0));
			circle(buttonMask2Bot, Point(buttonMask2Bot.size().width*3/4-15,buttonMask2Bot.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(buttonMask2Bot);
			thresholds[i].push_back(1000);

			Mat shutDownButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(shutDownButtonMask, Point(shutDownButtonMask.size().width-50,50), 30, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(shutDownButtonMask);
			thresholds[i].push_back(500);
		}
		else if (i==2){

			// Mat buttonMask0(projRect.size(), CV_8U, Scalar::all(0));
			// circle(buttonMask0, Point(buttonMask0.size().width/4+20,buttonMask0.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), -1);
			// buttonMasks[i].push_back(buttonMask0);
			// thresholds[i].push_back(1000);



		}
		else if (i==3){

			Mat videoOnOffMask(projRect.size(), CV_8U, Scalar::all(0));
			rectangle(videoOnOffMask, Rect(150, 150, videoOnOffMask.size().width-150*2, 200), Scalar(255,255,255), -1);
			rectangle(videoOnOffMask, Rect(200, 150, videoOnOffMask.size().width-200*2, 150), Scalar(0,0,0), -1);
			buttonMasks[i].push_back(videoOnOffMask);
			thresholds[i].push_back(1000);	


			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-50,backButtonMask.size().height-50), 30, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(1000);	
		}else if (i==4){
			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-50,backButtonMask.size().height-50), 30, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(1000);	
		}else if (i==5){
			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-50,backButtonMask.size().height-50), 30, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(1000);	
		}else if (i==6){
			Mat backButtonMask(projRect.size(), CV_8U, Scalar::all(0));
			circle(backButtonMask, Point(backButtonMask.size().width-50,backButtonMask.size().height-50), 30, Scalar(255,255,255), -1);
			buttonMasks[i].push_back(backButtonMask);
			thresholds[i].push_back(1000);	
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

	// int bottomPadding = 150;
	// int buttonRadius = 50;
	// circle(projFrame, Point(projFrame.size().width/4+20,projFrame.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), 2);
	// circle(projFrame, Point(projFrame.size().width/2+5,projFrame.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), 2);
	// circle(projFrame, Point(projFrame.size().width*3/4-15,projFrame.size().height-bottomPadding-buttonRadius), buttonRadius, Scalar(255,255,255), 2);

	// circle(projFrame, Point(projFrame.size().width/4+20,projFrame.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), 2);
	// circle(projFrame, Point(projFrame.size().width/2+5,projFrame.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), 2);
	// circle(projFrame, Point(projFrame.size().width*3/4-15,projFrame.size().height-buttonRadius), buttonRadius, Scalar(255,255,255), 2);

	// circle(projFrame, Point(projFrame.size().width-50,50), 30, Scalar(255,255,255), 2);

	rectangle(projFrame, Rect(150, 150, projFrame.size().width-150*2, 200), Scalar(255,255,255), 2);
	rectangle(projFrame, Rect(200, 150, projFrame.size().width-200*2, 150), Scalar(0,0,0), 2);
	circle(projFrame, Point(projFrame.size().width-50,projFrame.size().height-100), 30, Scalar(255,255,255), 2);

	imshow("projRegion", projFrame);

	Mat binary;
	// cout << initProjFrame.size() << endl;
	absdiff(initProjFrame, projFrame, binary);
	cvtColor(binary, binary, CV_BGR2GRAY);
	medianBlur(binary, binary, 9);
	inRange(binary, Scalar(30, 30, 30), Scalar(255, 255, 255), binary);
	medianBlur(binary, binary, 3);
	imshow("binary", binary);

	unsigned int buttonWhites = 0;
	unsigned int totalWhites = countNonZero(binary == 255);

	for (int i = 0; i < buttonMasks[status].size(); ++i)
	{
		Mat buttonDiff = binary & buttonMasks[status][i];
		int countWhite = countNonZero(buttonDiff == 255);
		buttonWhites += countWhite;
		if (countWhite > thresholds[status][i])
		{
			pushedButtons.push_back(1);
		}
		else
		{
			pushedButtons.push_back(0);
		}
	}

	cout << "totalWhites " << totalWhites << " buttonWhites " << buttonWhites;
	if ((totalWhites > 1000 && totalWhites > buttonWhites * 10) || totalWhites > 10000 ){
		cout << " reset frame";
		setInitFrame(newFrame);
	}
	cout << endl;

	return pushedButtons;
}