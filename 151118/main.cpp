#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#ifdef _WIN32
#include <windows.h>
#include "messenger.hpp"
#endif

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "main.hpp"
#include "detector.hpp"
#include "gesture.hpp"
#include "camera.hpp"

#ifdef __APPLE__
#include <sys/time.h>
#endif

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
	int camera_num = 0;
	if (argc >= 2){
		camera_num = argv[1][0]-'0';
	}
	Camera cam(camera_num);
	cam.cap.set(CV_CAP_PROP_FRAME_WIDTH, 10000);
	cam.cap.set(CV_CAP_PROP_FRAME_HEIGHT, 10000);
	uint32_t cam_width = (uint32_t) cam.cap.get(CV_CAP_PROP_FRAME_WIDTH);
	uint32_t cam_height = (uint32_t) cam.cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "camera number " << camera_num << " with width=" << cam_width << " and height=" << cam_height<< endl;
	Mat frame;

	Detector dtct;
	Gesture gest(cam_width, cam_height);

#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974);
#endif

	unsigned int tick;
	gesture::result res;
	tesseract::TessBaseAPI *api;

	api = new tesseract::TessBaseAPI();
	api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	for(;;){
		tick = getTick();

		cam.getImage(frame);
		Mat reducedFrame;
		Mat pageImg;
		pyrDown(frame, reducedFrame);

		dtct.detectBook(frame, frame, pageImg);
	
		api->SetImage((uchar*)pageImg.data, pageImg.cols, pageImg.rows, 1, pageImg.cols);
		char *outText = api->GetUTF8Text();
		int tempPageNum = atoi(outText);
		//if (tempPageNum>0) pageNum = tempPageNum;
		printf("OCR output:%s\n", outText);
		cout << "page number is: " << atoi(outText) << endl;
		delete [] outText;

		Point v = dtct.detectTip(frame);
		res = gest.registerPoint(v.x, v.y, tick);
		//gest.visualize(frame);
		imshow("ddd", pageImg);

#ifdef _WIN32
		if (res.type == gesture::V_TYPE)
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, res.V2_x, res.V2_y, 74);
		else
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, -1, -1, 69);
#endif
  		if(cv::waitKey(30) == char('q')) break;
	}
	destroyAllWindows();
	return 0;
}