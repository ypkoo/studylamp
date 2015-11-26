// main.cpp
// 
// Main code for recognition
// 
// @author S.H.Kang, S.H.Lee
// 
// @version 1.1
// @since 2015-11-26
// VideoCapture code appended corresponding removal of Camera object.
//
// @version 1.0
// @since 2015-11-XX
// First implementation

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#ifdef _WIN32
#include <windows.h>
#include "messenger.hpp"
#elif __APPLE__
#include <sys/time.h>
#endif

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "detector.hpp"
#include "gesture.hpp"


using namespace cv;
using namespace std;

unsigned int getTick(void) {
	unsigned int tick;
#ifdef _WIN32
	tick = GetTickCount();
#elif __APPLE__
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tick = tv.tv_usec / 1000 + tv.tv_sec * 1000;
#endif
	return tick;
}

int main(int argc, char **argv){
	/* Parsing arguments */
	int dev_num = 0;
	if (argc >= 2)
		sscanf(argv[1], "%d", &dev_num);

	/* Initialize camera device */
	VideoCapture VC(dev_num);
	uint32_t cam_width, cam_height;
	{
		if (!VC.isOpened()) {
			fprintf(stderr, "Failed to open device number with number %d\n", dev_num);
			return -1;
		}
		/* Set size as big enough, to get maximum size */
		VC.set(CV_CAP_PROP_FRAME_WIDTH, 800);
		VC.set(CV_CAP_PROP_FRAME_HEIGHT, 400);
		cam_width = (uint32_t) VC.get(CV_CAP_PROP_FRAME_WIDTH);
		cam_height = (uint32_t) VC.get(CV_CAP_PROP_FRAME_HEIGHT);
		printf("Camera number %d, (w, h) = (%d, %d)\n", dev_num, cam_width, cam_height);
	}

	Mat frame;
	Detector dtct;
	Gesture gest(cam_width, cam_height);

#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974);
#endif
	gesture::result res;
	tesseract::TessBaseAPI *api;

	api = new tesseract::TessBaseAPI();
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		return -1;
	}

	unsigned int tick;
	for(;;){
		tick = getTick(); // Get time.
		VC >> frame;      // Get current image.
		Mat reducedFrame;
		Mat pageImg;
		pyrDown(frame, reducedFrame);

		// dtct.detectBook(frame, frame, pageImg);
	
		// api->SetImage((uchar*)pageImg.data, pageImg.cols, pageImg.rows, 1, pageImg.cols);
		// char *outText = api->GetUTF8Text();
		// int tempPageNum = atoi(outText);
		// // if (tempPageNum>0) pageNum = tempPageNum;
		// printf("OCR output:%s\n", outText);
		// cout << "page number is: " << atoi(outText) << endl;
		// delete [] outText;

		Point v = dtct.detectTip(frame);
		res = gest.registerPoint(v.x, v.y, tick);
		gest.visualize(frame, tick);
		imshow("ddd", frame);

#ifdef _WIN32
		if (res.type == gesture::V_TYPE)
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, res.V2_x, res.V2_y, 74);
		else
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, -1, -1, 69);
#endif
  		if(cv::waitKey(30) == char('q')) break;
	}
	VC.release();
	destroyAllWindows();
	return 0;
}