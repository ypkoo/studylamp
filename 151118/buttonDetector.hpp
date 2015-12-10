#ifndef _BUTTONDETECTOR_HPP_
#define _BUTTONDETECTOR_HPP_

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "common.hpp"

using namespace cv;
using namespace std;

class ButtonDetector{
	public:
		ButtonDetector(unsigned int width, unsigned int height);
		vector<unsigned int> registerFrame(Mat newFrame);
		void setInitFrame(Mat initFrame);
		void setStatus(enum PROGRAM_STATUS newStatus);
	private:
		unsigned int width;
		unsigned int height;

		Mat initProjFrame;

		unsigned int frameChangeInitTick;
		unsigned int frameChangeCount;
		
		Rect projRect;

		enum PROGRAM_STATUS status;
		vector<vector<Mat> > buttonMasks;
		vector<vector<unsigned int> > lastInitTicks;
		vector<vector<unsigned int> > lastRecogTicks;
		vector<vector<unsigned int> > emptyButtonCounts;
		vector<vector<unsigned int> > thresholds;
		void init();

		void handleEvent(int evt, int x, int y, int flags);
		enum _grabStatus{
			GRAB_NOTHING,
			GRAB_LEFTTOP,
			GRAB_RIGHTTOP,
			GRAB_RIGHTBOTTOM,
			GRAB_LEFTBOTTOM
		} grabStatus;
};


#endif