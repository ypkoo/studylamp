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

Point linear_trans(Point v, double cosv, double sinv) {
	Point res;
	res.x = (int)(  cosv  * v.x + sinv * v.y);
	res.y = (int)((-sinv) * v.x + cosv * v.y);
	return res;
}

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
	uint32_t cam_width = 0, cam_height = 0;
	{
		if (!VC.isOpened()) {
			fprintf(stderr, "Failed to open device number with number %d\n", dev_num);
			return -1;
		}
		/* Set size as big enough, to get maximum size */
		VC.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
		VC.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
		// VC.set(CV_CAP_PROP_FRAME_WIDTH, 2592);
		// VC.set(CV_CAP_PROP_FRAME_HEIGHT, 1944);
		cam_width = (uint32_t) VC.get(CV_CAP_PROP_FRAME_WIDTH);
		cam_height = (uint32_t) VC.get(CV_CAP_PROP_FRAME_HEIGHT);
		printf("Camera number %d, (w, h) = (%d, %d)\n", dev_num, cam_width, cam_height);
	}

	Detector dtct;
	Gesture gest(cam_width, cam_height);
	gesture::result res;

#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974);
#endif

	Mat frame;
	VC>>frame;
	// imwrite("output.bmp", frame);

	for(;;) {
		VC >> frame;

		Mat bookImg;
		int pageNum;
		Point abspoint, relpoint;
		dtct.detect(frame, bookImg, pageNum, relpoint);

		abspoint = dtct.rel2abs(relpoint);
		res = gest.registerPoint(abspoint, getTick());
#ifdef DEBUG
		gest.visualize(frame, getTick());
		circle(frame, abspoint, 5, Scalar(0, 0, 255), CV_FILLED);
		imshow("Gest", frame);
#endif

#ifdef _WIN32
		if (res.type == gesture::V_TYPE) {
			Point vpoint(res.V2_x, res.V2_y);
			Point avpoint = dtct.rel2abs(vpoint);
			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
				bookImg.cols, bookImg.rows,
				relpoint.x, relpoint.y, vpoint.x, vpoint.y, 41);
		}
		else
			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
				bookImg.cols, bookImg.rows,
				relpoint.x, relpoint.y, -1, -1, 41);
#endif
  		if(cv::waitKey(30) == char('q')) break;
	}
	VC.release();
	destroyAllWindows();
	return 0;
}