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
#include <inttypes.h>
#include "common.hpp"
#include "detector.hpp"
#include "gesture.hpp"
#include "buttonDetector.hpp"
#include "settingLoader.hpp"

// #define HAVE_IMAGE_HASH
// #define cimg_use_jpeg
// #define cimg_use_png

// // #include <pHash.h>
// #include "phash/pHash.h"
#include <thread>

using namespace cv;
using namespace std;

char phash_file[100] = "phash.jpg";

PROGRAM_STATUS udp_state;
uint32_t udp_last_tick = 0;

Point linear_trans(Point v, double cosv, double sinv) {
	Point res;
	res.x = (int)(  cosv  * v.x + sinv * v.y);
	res.y = (int)((-sinv) * v.x + cosv * v.y);
	return res;
}

// receiving UDP message
#ifdef _WIN32
#define UDP_BUFFER_SIZE 1024
void update_udp_state(Messenger *msg){
	int bytes_read;
	char buf[UDP_BUFFER_SIZE];
	char res;
	while (1) {
		bytes_read = msg->receive_message(buf, UDP_BUFFER_SIZE);
		if (bytes_read == UDP_BUFFER_SIZE) {
			fprintf(stderr, "ERROR: message received from udp too much");
			exit(1);
		}
#ifdef DEBUG
		buf[bytes_read] = 0;
		printf("[UDP RECEIVED] %s\n", buf);
#endif
		res = buf[bytes_read-1]-'0';
		if (res != (char) udp_state
			&& 0 <= res && res <= STATUS_MAX
			&& getTick() - udp_last_tick > 400) { /* state change needs 1 second */
			udp_last_tick = getTick(); 
			udp_state = (PROGRAM_STATUS) res;
		}
	}
}
#endif

// phash function
void myphash(Mat src, uint64_t &hash){
	// Mat src, dst, dst2, kernel;
	Mat dst, dst2, kernel;
	Mat chan[3];
	vector<float> vec;

	// src = imread (file);
	// src = imread (file);
	cvtColor(src, src, CV_BGR2YCrCb);

	split(src, chan);

	kernel = Mat::ones(7, 7, CV_32F) / (float)(49);
	filter2D(chan[0], dst, -1, kernel, Point(-1, -1), 0, BORDER_REPLICATE); // BORDER_REPLICATE
	resize(dst, dst, Size(32, 32), 0, 0, INTER_NEAREST); // INTER_NEAREST

	dst.convertTo(dst2, CV_32FC1);
	dct(dst2, dst2);

	/* Get (1, 1) ~ (8, 8) rectangle */
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			vec.push_back(dst2.at<float>(i+1, j+1)); // k = 1
		}
	}

	/* Get Median */
	vector<float> med_sorted(vec);
	nth_element(med_sorted.begin(), med_sorted.begin()+31, med_sorted.end());
	float median = med_sorted[31];

	/* TEST */
	// for (int i = 0; i <64; i++)
	// 	printf("%2d %+4.4f %+4.4f\n", i, vec[i], med_sorted[i]);

	/* Calculate phash value */
	uint64_t one = 1;
	hash = 0;
	for (int i=0; i<64; i++) {
		float current = vec[i];
		if (current > median)
			hash |= one;
		one = one << 1;
	}
}

/* To get evaluation time for each functions. */
uint32_t prevTime = 0;
void debug_time(char *func, bool last) {
	uint32_t new_tick = getTick();
	// printf("%s:%7d%c", func, new_tick - prevTime, (last) ? '\n':' ');
	prevTime = new_tick;
}

int main(int argc, char **argv){
	setting_init();
	int dev_num = setting_load_u32 ("device_num", 0);
	int loop_period = setting_load_u32 ("loop_minimum", 0);
	int send_msg_period = setting_load_u32 ("send_msg_period", 100);

	/* Initialize camera device */
	VideoCapture VC(dev_num);
	uint32_t cam_width = 0, cam_height = 0;
	{
		if (!VC.isOpened()) {
			fprintf(stderr, "Failed to open device number '%d' for camera\n", dev_num);
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
	udp_state = (PROGRAM_STATUS) setting_load_u32 ("initial_state", 0);
#ifdef _WIN32
	Messenger msg("127.0.0.1", setting_load_u32("send_port", 6974), setting_load_u32("recv_port", 7469));
	udp_last_tick = getTick();
	thread updating_thread(&update_udp_state, &msg);
	updating_thread.detach();
#endif

	/* Objects */
	Mat frame; // Original camera frame.
	gesture::result res;
	PROGRAM_STATUS program_status = STATUS_BOOKCOVER;
	bool loop = true;
	uint64_t hash_val;
	uint32_t bWidth, bHeight;  // book width and height
	uint32_t tipX, tipY;       // x, y coordinate of the tip
	uint32_t checkX, checkY;   // x, y coordinate of checked location
	int32_t pageNum;
	char key;                  // returned value from waitKey(30)

	/* Some processes before the loop */
	VC>>frame;
	int loop_tick = getTick();
	int send_tick = getTick();
	bd.setInitFrame(frame);

	/* Main loop */
	while(loop) {
		/* Program wait for loop_period. */
		if (getTick() - loop_tick < loop_period)
			continue;
		else
			loop_tick = getTick();

debug_time("s", 0);
		/* Status changed */
		if (program_status != udp_state){
			program_status = udp_state;
			bd.setStatus(program_status);
			bd.setInitFrame(frame);	
		}

		VC >> frame;
		imshow("Original", frame);

		Mat bookImg;
		Point abspoint, relpoint;
		Mat gestFrame;

		switch (program_status) {
			case STATUS_BOOKCOVER:     // 0
				/* PHASH */
				dtct.detect(frame, bookImg, pageNum, relpoint);
				imshow("bookImg", bookImg);
				// imwrite("bookImg.png", bookImg);
				myphash(bookImg, hash_val);
				// printf("hash_value %" PRIu64 "\n", hash_val);
#ifdef _WIN32
				msg.send_message("%d;%" PRIu64 , (int) program_status, hash_val);
				// msg.print_buf();
#endif
				key = cv::waitKey (30);
				if (key == 'q')
					loop = false;
				continue;
			case STATUS_STUDY_LEARNING:  // 2
			case STATUS_STUDY_SOLVING: // 3
			case STATUS_STUDY_SOLVED:  // 4
			case STATUS_BUFFER:        // 7. It needs to send page number.
debug_time("befdet", 0);
				dtct.detect(frame, bookImg, pageNum, relpoint);
debug_time("det", 0);

				abspoint = dtct.rel2abs(relpoint);
				res = gest.registerPoint(abspoint, getTick());
debug_time("reg", 0);
#ifdef DEBUG
				gestFrame = frame.clone();
				gest.visualize(gestFrame, getTick());
				circle(gestFrame, abspoint, 5, Scalar(0, 0, 255), CV_FILLED);
				imshow("Gest", gestFrame);
#endif
				bWidth = bookImg.cols; bHeight = bookImg.rows;
				tipX = relpoint.x; tipY = relpoint.y;
				if (res.type == gesture::V_TYPE) {
					Point relCheckedPoint = dtct.abs2rel(Point(res.V2_x, res.V2_y));
					checkX = relCheckedPoint.x; checkY = relCheckedPoint.y;
				}
				else {
					checkX = -1; checkY = -1;
				}
debug_time("etc", 0);


				break;

			case STATUS_MAINMENU:        // 1
			case STATUS_PROGRESS:        // 5
			case STATUS_REVIEW:          // 6
				/* Button processing */
				bWidth = bHeight = tipX = tipY = checkX = checkY = pageNum = 0;
		}

		// Button processing part
		// this part of code can't be accessed for only STATUS_BOOKCOVER
		stringstream button_ss;
		vector<unsigned int> buttons = bd.registerFrame(frame);
		copy(buttons.begin(), buttons.end(), ostream_iterator<int>(button_ss));
debug_time("but", 1);

#ifdef _WIN32
		msg.send_message("%d;%d;%d;%d;%d;%d;%d;%d;%s", (int)program_status, bWidth, bHeight, tipX, tipY, checkX, checkY, pageNum, button_ss.str().c_str());
		// msg.print_buf();
#endif


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
		key = cv::waitKey(30);
		if (key == 'q')
			loop = false;
	}
#ifdef _WIN32
	TerminateThread(updating_thread.native_handle(), 0);
	WaitForSingleObject(updating_thread.native_handle(), INFINITE);
#endif
	destroyAllWindows();
	VC.release();
	return 0;
}