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

// #define HAVE_IMAGE_HASH
// #define cimg_use_jpeg
// #define cimg_use_png

// // #include <pHash.h>
// #include "phash/pHash.h"



using namespace cv;
using namespace std;

#define UDP_BUFFER_SIZE 1024
char phash_file[100] = "phash.jpg";

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
	bd.setInitFrame(frame);
	// imwrite("output.bmp", frame);

	/* Main loop */
	bool loop = true;
	while(loop) {
		/* Receive message, to catch whether state changed or not. */
#ifdef _WIN32
		int bytes_read = 0;
		char last_char = '\0';
		bytes_read = msg.receive_message(udp_buffer, UDP_BUFFER_SIZE);
		if (bytes_read == UDP_BUFFER_SIZE) {
			fprintf(stderr, "udp receive too much");
			exit(1);
		}
		if (bytes_read != -1){
#ifdef DEBUG
			udp_buffer[bytes_read] = 0;
			cout << "udp received: " << udp_buffer << endl;
#endif
			program_status = (enum PROGRAM_STATUS) (udp_buffer[bytes_read-1] -'0');
		}
#elif __APPLE__ // MACBOOK case
		program_status = STATUS_STUDY_SOLVING; // to get gesture things.
#endif
		if(cv::waitKey(30) == '0')
			program_status = (PROGRAM_STATUS) 0;
		if(cv::waitKey(30) == '1')
			program_status = (PROGRAM_STATUS) 1;
		if(cv::waitKey(30) == '2')
			program_status = (PROGRAM_STATUS) 2;
		if(cv::waitKey(30) == '3')
			program_status = (PROGRAM_STATUS) 3;
		if(cv::waitKey(30) == '4')
			program_status = (PROGRAM_STATUS) 4;
		if(cv::waitKey(30) == '5')
			program_status = (PROGRAM_STATUS) 5;
		if(cv::waitKey(30) == '6')
			program_status = (PROGRAM_STATUS) 6;
		if(cv::waitKey(30) == '7')
			program_status = (PROGRAM_STATUS) 7;
		bd.setStatus(program_status);

		VC >> frame;
		imshow("frameImg", frame);

		Mat bookImg;
		int pageNum;
		Point abspoint, relpoint;
		Mat gestFrame;

		uint64_t hash_val;
		String tmp;

		udp_buffer2.str("");
		udp_buffer2 << (int) program_status << ';';

		switch (program_status) {
			case STATUS_BOOKCOVER:     // 0
				/* PHASH */
				dtct.detect(frame, bookImg, pageNum, relpoint);
				imshow("bookImg", bookImg);
				// imwrite("bookImg.png", bookImg);
				myphash(bookImg, hash_val);
			 	udp_buffer2 << hash_val;
#ifdef _WIN32
				tmp = udp_buffer2.str();
				for (int _index=0;_index<40;_index++) {
					if (hash_val & (1<<_index))
						cout << "*";
					else
						cout << " ";
				}
				cout << hash_val << endl;
				msg.send_message(tmp.c_str(), tmp.length());
#endif
  				if(cv::waitKey(30) == char('q'))
  					loop = false;
				continue;
			case STATUS_STUDY_SOLVING: // 3
			case STATUS_STUDY_SOLVED:  // 4
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
			case STATUS_BUFFER:          // 7
				/* Button processing */
				udp_buffer2 << "0;0;0;0;0;0;0;";

		}

		// this part of code can't be accessed for only STATUS_BOOKCOVER
		vector<unsigned int> buttons = bd.registerFrame(frame);
		int i;
		for (i = 0; i < buttons.size(); i++)
			udp_buffer2 << buttons[i];

#ifdef _WIN32
		tmp = udp_buffer2.str();
		msg.send_message(tmp.c_str(), tmp.length());
#endif
		udp_buffer2.str("");


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
  		if(cv::waitKey(30) == char('q'))
  			loop = false;
	}
	VC.release();
	destroyAllWindows();
	return 0;
}