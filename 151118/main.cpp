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

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
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
	uint32_t cam_width = 0, cam_height = 0;
	{
		if (!VC.isOpened()) {
			fprintf(stderr, "Failed to open device number with number %d\n", dev_num);
			return -1;
		}
		/* Set size as big enough, to get maximum size */
		VC.set(CV_CAP_PROP_FRAME_WIDTH, 100000);
		VC.set(CV_CAP_PROP_FRAME_HEIGHT, 100000);
		cam_width = (uint32_t) VC.get(CV_CAP_PROP_FRAME_WIDTH);
		cam_height = (uint32_t) VC.get(CV_CAP_PROP_FRAME_HEIGHT);
		printf("Camera number %d, (w, h) = (%d, %d)\n", dev_num, cam_width, cam_height);
	}

	Detector dtct;
	Gesture gest(cam_width, cam_height);
	unsigned int tick;
	gesture::result res;

#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974);
#endif

	tesseract::TessBaseAPI *api;
	api = new tesseract::TessBaseAPI();
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	Mat frame;
	VC>>frame;
	imwrite("output.bmp", frame);

	Mat bookRotMat, rotMat;
	Point bookCoord;
	float cosval, sinval;
	for(;;) {
		tick = getTick();

		VC >> frame;
		//pyrDown(frame, frame);

		Mat bookImg, pageImg;
		dtct.detectBook(frame, bookImg, pageImg, bookRotMat, bookCoord);

		api->SetImage((uchar*)pageImg.data, pageImg.cols, pageImg.rows, 1, pageImg.cols);
		char *outText = api->GetUTF8Text();
		cout << outText[0] << outText[1] << endl;
		// delete [] outText;

		Point v = dtct.detectTip(bookImg);

		cosval = bookRotMat.at<double>(0, 0);
		sinval = bookRotMat.at<double>(0, 1);
		cout << bookRotMat << cosval << sinval << endl;
		cout << bookCoord << endl;
		rotMat = (Mat_<double>(2, 2) << cosval, sinval, -sinval, cosval);
		v = transform(v, v, rotMat.inv());
		v += bookCoord;
		res = gest.registerPoint(v.x, v.y, tick);
#ifdef DEBUG
		Mat gestmat(bookImg);
		gest.visualize(gestmat, tick);
		circle(gestmat, rotMat.inv() *(v-bookCoord), 5, Scalar(0, 0, 255), CV_FILLED);
		imshow("Gest", gestmat);
#endif
		// imshow("Original", bookImg);

#ifdef _WIN32
		if (res.type == gesture::V_TYPE)
			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
				bookImg.cols, bookImg.rows,
				v.x, v.y, res.V2_x, res.V2_y, 41);
		else
			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
				bookImg.cols, bookImg.rows,
				v.x, v.y, -1, -1, 41);
#endif
  		if(cv::waitKey(30) == char('q')) break;
	}
	VC.release();
	destroyAllWindows();
	return 0;
}