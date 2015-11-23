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
#include "trackFinger.hpp"
#include "gesture.hpp"
#include "camera.hpp"
#include "imageView.hpp"

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

	cout << "camera number " << camera_num << endl;

	Gesture gest;
	TrackFinger tf(true);

#ifdef _WIN32
	Messenger msg("127.0.0.1", 6974);
#endif

	Camera cam(camera_num);
	ImageView iv_orig("original");
	ImageView iv_trig("triggering");

	Mat img;
	unsigned int tick;
	gesture::result res;

	for(;;){
		tick = getTick();
		cam.getImage(img);

		Point v = tf.getFingerPoint(img);
		res = gest.registerPoint(v.x, v.y, tick);

#ifdef _WIN32
		if (res.type == gesture::V_TYPE)
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, res.V2_x, res.V2_y, 74);
		else
			msg.send_message ("%d;%d;%d;%d;%d", v.x, v.y, -1, -1, 69);
#endif
		// cout << v << endl;

		iv_orig.showImage(img);

		//gest.visualize(img);
		iv_trig.showImage(img);
		
		// showWindows(m);
  		if(cv::waitKey(30) == char('q')) break;
	}
	destroyAllWindows();
	return 0;
}