#include "gesture.hpp"
#include <iostream>
#include <string>
#include <cmath>

using namespace gest;
using namespace cv;
using namespace std;

gesture::gesture()
{
	points = (struct tp *) malloc(sizeof(struct tp) * POINT_COUNT);
	clusters = (struct cluster *) malloc(sizeof(struct cluster) * CLUSTER_COUNT);
	lines = (struct line *) malloc(sizeof(struct line) * (CLUSTER_COUNT-1));
	pindex = 0;
	cindex = 0;
}

void gesture::registerPoint(uint32_t x, uint32_t y, uint32_t t)
{
	// if buffer becomes to be full, remove all clusters except the last three clusters.
	if (pindex == POINT_COUNT - 2 || cindex == CLUSTER_COUNT - 2) {
		cout << "gesture.h: cleared" << endl;
		clear();
	}

	if (pindex > 0){
		points[pindex].x = x;
		points[pindex].y = y;
		points[pindex].t = t;

		uint32_t time_diff = t - points[pindex-1].t;
		
		if (time_diff > TIME_LIMIT){
			clear();
		}
		// TIME IN
		else{
			// CLUSTERING
			if (clusters[cindex].end == pindex-1){
				uint32_t count = clusters[cindex].count;
				uint32_t xsq = (clusters[cindex].cx - x) * (clusters[cindex].cx - x);
				uint32_t ysq = (clusters[cindex].cy - y) * (clusters[cindex].cy - y);

				// enoughly NEAR, just enough to CLUSTER.
				if (xsq + ysq < SIZE_LIMIT_SQUARE){
					clusters[cindex].cx *= count;
					clusters[cindex].cx += x;
					clusters[cindex].cx /= count+1;
					clusters[cindex].cy *= count;
					clusters[cindex].cy += y;
					clusters[cindex].cy /= count+1;
					clusters[cindex].duration += time_diff; // cluster could completed at here.
					clusters[cindex].end++;
					clusters[cindex].count++;

					if (clusters[cindex].duration > TIME_TO_CLUSTER && cindex > 0){
						updateLine (cindex-1);
						check ();
					}
				} else { // so FAR
					// already made cluster
					if (clusters[cindex].duration > TIME_TO_CLUSTER) 
						cindex ++;

					clusters[cindex].cx = x;
					clusters[cindex].cy = y;
					clusters[cindex].duration = 0;
					clusters[cindex].start = clusters[cindex].end = pindex;
					clusters[cindex].count = 1;
				}
			}
		}
	}
	if (pindex == 0) {
		points[pindex].x = x;
		points[pindex].y = y;
		points[pindex].t = t;

		clusters[0].cx = x;
		clusters[0].cy = y;
		clusters[0].duration = 0;
		clusters[0].start = clusters[0].end = 0;
		clusters[0].count = 1;
	}
	
	pindex++;
}
void gesture::clear (void)
{
	memset (points, 0, sizeof(struct tp) * POINT_COUNT);
	memset (clusters, 0, sizeof(struct cluster) * CLUSTER_COUNT);
	memset (lines, 0, sizeof(struct line) * (CLUSTER_COUNT-1));
	cindex = pindex = 0;
}

void gesture::updateLine (size_t index)
{
	struct line *line = &lines[index];

	struct cluster *from = &clusters[index];
	struct cluster *to = &clusters[index+1];


	uint32_t s1, s2;
	float x1, x2, y1, y2;
	s1 = from->end - from->start + 1;
	s2 = to->end - to->start + 1;

	x1 = from->cx; x2 = to->cx;
	y1 = from->cy; y2 = to->cy;

	line->a = y2-y1;
	line->b = x1-x2;
	line->c = x2*y1-x1*y2;

	if (line->b != 0)
		line->slope = -line->a/line->b;
	else
		line->slope = 10000000.0;

	line->length = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
	line->duration = points[to->start-1].t - points[from->end+1].t;

	// If there is NO points between two clusters, prevent zero division.
	if (to->start - from->end <= 1) {
		line->is_linear = true;
		return;
	}

	uint32_t tolerance = 0;
	uint32_t t_limit = line->duration * 2 / 4;

	float tmp;
	uint32_t i;
	bool failed = false;
	float limit = sqrt((line->a)*(line->a) + (line->b)*(line->b))*line->length;
	for (i=from->end+1; i<to->start && !failed; i++){
		tmp = line->a * points[i].x + line->b * points[i].y + line->c;
		if (tmp < -limit || tmp > limit || 
			((x1+x2)/2-points[i].x)*((x1+x2)/2-points[i].x)+((y1+y2)/2-points[i].y)*((y1+y2)/2-points[i].y)
				> (line->length/2)*(line->length/2)) {
			tolerance += points[i+1].t - points[i-1].t;
			if (tolerance > t_limit)
				failed = true;
		}
	}
	line->is_linear = !failed;

	// int i; long long unsigned int sum = 0; int tmp;
	// for (i=from->end+1; i<to->start; i++) {
	// 	tmp = line->a * points[i].x + line->b * points[i].y + line->c;
	// 	sum += tmp*tmp;
	// }

	// line->error = sqrt ((float)sum / (float) ((to->start-from->end-1)*(line->a * line->a + line->b * line->b)));
}

void gesture::check (void) {
	// cout << "check " << lines[cindex-2].slope << ", " 
	// << lines[cindex-1].slope << endl;
	if (cindex > 1
		&& lines[cindex-2].slope > 0.3 && lines[cindex-2].slope < 3
		&& lines[cindex-1].slope > -3 && lines[cindex-1].slope < -0.3
		&& lines[cindex-2].is_linear
		&& lines[cindex-1].is_linear) {
		cout << "gesture.h: V shape finded: ("
			<< clusters[cindex-1].cx << ", "
			<< clusters[cindex-1].cy << ")" << endl;
		clear();
	}
}

void gesture::visualize (Mat& img) {
	size_t i;
	for (i = 0; i < cindex; i++){
		if (clusters[i].duration > TIME_TO_CLUSTER) {
			circle(img,
				Point(clusters[i].cx,
					clusters[i].cy),
				8, Scalar(255, 0, 0), CV_FILLED);
			if (i > 0 && lines[i-1].is_linear) {
				cv::line(img,
					Point (clusters[i-1].cx,
						clusters[i-1].cy),
					Point (clusters[i].cx,
						clusters[i].cy),
					Scalar(40, 255, 40),
					4);
			}
		}
	}
}