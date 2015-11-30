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
#include <sstream>
#include "common.hpp"
#include "detector.hpp"
#include "gesture.hpp"
#include "buttonDetector.hpp"

#define HAVE_IMAGE_HASH
#include <pHash.h>



using namespace cv;
using namespace std;

#define UDP_BUFFER_SIZE 1024

Point linear_trans(Point v, double cosv, double sinv) {
	Point res;
	res.x = (int)(  cosv  * v.x + sinv * v.y);
	res.y = (int)((-sinv) * v.x + cosv * v.y);
	return res;
}

// Get current time tick, with unit of milliseconds.
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

	/* Other modules. */
	Detector dtct;
	ButtonDetector bd(cam_width, cam_height);
	Gesture gest(cam_width, cam_height);
#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974, 7469);
#endif

	/* Objects */
	Mat frame; // Original camera frame.
	gesture::result res;
	PROGRAM_STATUS program_status = STATUS_BOOKCOVER;
	char udp_buffer[UDP_BUFFER_SIZE];
	stringstream udp_buffer2;

	/* Some processes before the loop */
	VC>>frame;
		cout << 0 << endl;
	bd.setInitFrame(frame);
	// imwrite("output.bmp", frame);
		cout << 0 << endl;

	/* Main loop */
	while(1) {
		/* Receive message, to catch whether state changed or not. */
		cout << 0 << endl;
#ifdef _WIN32
		int bytes_read = 0;
		char last_char = '\0';
		bytes_read = msg.receive_message(udp_buffer, UDP_BUFFER_SIZE);
		if (bytes_read == UDP_BUFFER_SIZE) {
			fprintf(stderr, "udp receive too much");
			exit(1);
		}
		if (bytes_read != 0){
#ifdef DEBUG
			udp_buffer[bytes_read] = 0;
			cout << "udp received: " << udp_buffer << endl;
#endif
			program_status = (enum PROGRAM_STATUS) (udp_buffer[bytes_read-1] -'0');
		}
#elif __APPLE__ // MACBOOK case
		program_status = STATUS_STUDY_SOLVING; // to get gesture things.
#endif


		cout << (int)program_status << endl;
		VC >> frame;

		Mat bookImg;
		int pageNum;
		Point abspoint, relpoint;
		Mat gestFrame;

		int hash_res;


		switch (program_status){
			case STATUS_BOOKCOVER:
				udp_buffer2 << 0 << ';';
				break;
			case STATUS_MAINMENU:
				udp_buffer2 << 1 << ';';
				break;
			case STATUS_STUDY_LEARNING:
			case STATUS_STUDY_SOLVING:
			case STATUS_STUDY_SOLVED:
				udp_buffer2 << 2 << ';';
				break;
			case STATUS_PROGRESS:
				udp_buffer2 << 3 << ';';
				break;
			case STATUS_REVIEW:
				udp_buffer2 << 4 << ';';
				break;
		}

		switch (program_status) {
    		case STATUS_BOOKCOVER:     // 0
    			/* PHASH */
				dtct.detect(frame, bookImg, pageNum, relpoint);
    			imwrite ("phash.png", bookImg);
    			ulong64 hs;
    			hash_res = ph_dct_imagehash("phash.png", hs);
    			udp_buffer2 << hs;
    			continue;
    		case STATUS_STUDY_SOLVING: // 3
    		case STATUS_STUDY_SOLVED:  // 4
    			/* Processing on book, and gesture things.
    			   Break should NOT be in this part,
    			   since button processing also must be done in these states. */

				dtct.detect(frame, bookImg, pageNum, relpoint);

				abspoint = dtct.rel2abs(relpoint);
				res = gest.registerPoint(abspoint, getTick());
#ifdef DEBUG
				gestFrame = frame.clone();
				gest.visualize(gestFrame, getTick());
				circle(gestFrame, abspoint, 5, Scalar(0, 0, 255), CV_FILLED);
				imshow("Gest", gestFrame);
#endif
				udp_buffer2 << bookImg.cols << ";"
					<< bookImg.rows << ";"
					<< relpoint.x << ";"
					<< relpoint.y << ";";
				if (res.type == gesture::V_TYPE){
					Point vpoint(res.V2_x, res.V2_y);
					Point avpoint = dtct.rel2abs(vpoint);
					udp_buffer2 << vpoint.x << ";" << vpoint.y << ";";
				}
				else {
					udp_buffer2 << "-1;-1;";
				}
				udp_buffer2 << pageNum << ";";
				break;

			case STATUS_MAINMENU:        // 1
			case STATUS_STUDY_LEARNING:  // 2
			case STATUS_PROGRESS:        // 5
			case STATUS_REVIEW:          // 6
				/* Button processing */
				udp_buffer2 << "0;0;0;0;0;0;0;";

		}

		// this part of code can't be accessed for only STATUS_BOOKCOVER
		vector<unsigned int> buttons = bd.registerFrame(frame);
		int i;
		for (i = 0; i < buttons.size(); i++)
			udp_buffer2 << buttons[i];
		string tmp = udp_buffer2.str();

#ifdef _WIN32
		msg.send_message(tmp.c_str(), tmp.length());
#endif
		udp_buffer2.clear();


// #ifdef _WIN32
// 		if (res.type == gesture::V_TYPE) {
// 			Point vpoint(res.V2_x, res.V2_y);
// 			Point avpoint = dtct.rel2abs(vpoint);
// 			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
// 				bookImg.cols, bookImg.rows,
// 				relpoint.x, relpoint.y, vpoint.x, vpoint.y, 41);
// 		}
// 		else
// 			msg.send_message ("%d;%d;%d;%d;%d;%d;%d",
// 				bookImg.cols, bookImg.rows,
// 				relpoint.x, relpoint.y, -1, -1, 41);
// #endif
  		if(cv::waitKey(30) == char('q')) break;
	}
	VC.release();
	destroyAllWindows();
	return 0;
}