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
		ButtonDetector();
		vector<unsigned int> registerFrame(Mat newFrame);
		void setInitFrame(Mat initFrame);
		void setStatus(enum PROGRAM_STATUS newStatus);
	private:
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
};


#endif