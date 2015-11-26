// gesture.cpp
// 
// Gesture class
// Analyze continuous points set
// 
// @author S.H.Lee
// 
// @version 1.2
// @since 2015-11-26
// Strengthen visualize()
//
// @version 1.1
// @since 2015-11-23
// Modified code for more natural recognition.
// Before this, all three points that constructs V-shape are made with long time.
// At this moment, only the first point is made with long time.
//
// @version 1.0
// @since 2015-11-19
// First implementation

#include "gesture.hpp"
#include <iostream>
#include <string>
#include <cmath>

using namespace gesture;
using namespace cv;
using namespace std;

Gesture::Gesture(uint32_t w, uint32_t h)
{

	points = (struct timePoint *) malloc(sizeof(struct timePoint) * POINT_COUNT);
	p_index = 0;

	GROUP_SIZE_LIMIT_SQUARE = h*h/(120 * 120);
	GROUP_DETAILED_SIZE_LIMIT_SQUARE = h*h/(150 * 150);
	EPSILON = h/200.0;

#ifdef DEBUG
	size_t *buf;
	size_t index;
	buf = (size_t *) malloc(sizeof(size_t) * HISTORY_MAX_COUNT * HISTORY_VECTOR_SIZE);
	for (index = 0; index < HISTORY_MAX_COUNT; index++) {
		history_vectors[index] = buf;
		buf += HISTORY_VECTOR_SIZE;
	}
	history_ticks = (uint32_t *) malloc(sizeof(uint32_t) * HISTORY_MAX_COUNT);
	history_count = 0;
#endif
}

Gesture::~Gesture()
{
	free(points);

#ifdef DEBUG
	free (history_vectors[0]);
	free (history_ticks);
#endif
}

void Gesture::DouglasPeucker(std::vector<size_t>& _marked, size_t front, size_t end){
	float dmax = 0; size_t index = 0;
	size_t i;
	int32_t A, B, C;
	float d, sqABsq;
	int32_t x, y;

	// equation of line L:Ax+By+C=0, which passes through points[front] and point[end]
	A = points[end].y - points[front].y;
	B = points[front].x - points[end].x;
	C = points[end].x*points[front].y - points[front].x*points[end].y;
	sqABsq = sqrt(A*A+B*B);

	for (i = front+1; i < end; i++){
		// get distance between points[i] and line L.
		x = points[i].x;
		y = points[i].y;
		d = abs(A*x+B*y+C)/sqABsq;
		if (d > dmax) {
			index = i;
			dmax = d;
		}
	}

	if (dmax > EPSILON) {
		if (index != front && index != end)
			_marked.push_back(index);
		if (index - front > 1)
			DouglasPeucker(_marked, front, index);
		if (end - index > 1)
			DouglasPeucker(_marked, index, end);
	}
}

result Gesture::registerPoint(int32_t x, int32_t y, uint32_t t)
{
	// cout << "gesture called" << endl;
	// if buffer becomes to be full, remove all groups except the last three groups.
	if (groups.size() == groups.max_size()) {
		cout << "gesture.cpp: groups size exceeded, fatal error" << endl;
		exit (-1);
	}
	if (p_index == POINT_COUNT) {
		cout << "gesture.cpp: points buffer are cleaned" << endl;
#ifdef DEBUG
		memset(history_vectors[0], 0, sizeof(size_t) * HISTORY_MAX_COUNT * HISTORY_VECTOR_SIZE);
		memset(history_ticks, 0, sizeof(uint32_t) * HISTORY_MAX_COUNT);
		history_count = 0;
#endif
		if (groups.size() == 0){
			memset(points, 0, sizeof(struct timePoint)*(p_index-1));
			p_index = 0;
		}
		else {
			size_t first = groups[0].start;
			memmove(points, &points[first], sizeof(struct timePoint)*(p_index-first-1));
			size_t index = 0;
			for (index = 0; index < groups.size(); index++) {
				groups[index].start -= first;
				groups[index].end -= first;
			}
			p_index -= first;
		}
	}

	if (x >= 0 && y >= 0) {
		// process new point
	// cout << "group size " << groups.size() << endl;
		if (!groups.empty()){
			struct group *end_gp = &groups[groups.size()-1];
			uint32_t count = end_gp->count;
			//cout << count << end_gp->x << ""endl;
			float xdiff = end_gp->x - x;
			float ydiff = end_gp->y - y;
			float diff = xdiff * xdiff + ydiff * ydiff;

			if (end_gp->duration < TIME_TO_GROUP) { // If the last group does not complete yet,
				if (diff < GROUP_SIZE_LIMIT_SQUARE) { // If near enough, just attach new point to group.
					end_gp->x = ((end_gp->x*count)+x)/(count+1);
					end_gp->y = ((end_gp->y*count)+y)/(count+1);
					end_gp->duration +=  t-points[end_gp->end].t;
					end_gp->end = p_index;
					end_gp->count++;
					end_gp->collect_time_limit = t + TIME_TO_MOTION;
				}
				else { // If so far, delete original group and make a new group.
					end_gp->x = x;
					end_gp->y = y;
					end_gp->duration = 0;
					end_gp->start = end_gp->end = p_index;
					end_gp->count = 1;
					end_gp->collect_time_limit = 0; // it is not meaningful to set.
				}
			}
			else { // If the last group was already complete,
				if (diff < GROUP_DETAILED_SIZE_LIMIT_SQUARE) { // If near enough, just attach new point with dequeue.
					end_gp->x += ((float)x-points[end_gp->start].x)/count;
					end_gp->y += ((float)y-points[end_gp->start].y)/count;
					// end_gp->duration = TIME_TO_GROUP;
					end_gp->start++;
					end_gp->end++;
					end_gp->collect_time_limit = t + TIME_TO_MOTION;
				}
				else { // If so far, make a new group. (* ready to track following points)
					struct group new_gp;
					new_gp.x = x;
					new_gp.y = y;
					new_gp.duration = 0;
					new_gp.start = new_gp.end = p_index;
					new_gp.count = 1;
					new_gp.collect_time_limit = 0; // it is not meaningful to set.
					groups.push_back(new_gp);
				}
			}
		}
		else {
			// if there was no group
			struct group new_gp;
			new_gp.x = x;
			new_gp.y = y;
			new_gp.duration = 0;
			new_gp.start = new_gp.end = p_index;
			new_gp.count = 1;
			new_gp.collect_time_limit = 0; // it is not meaningful to set.
			groups.push_back(new_gp);
		}

		// register new point
		points[p_index].x = x;
		points[p_index].y = y;
		points[p_index].t = t;

		p_index++;
	}
	// motion should be handled, whenever new points are accepted or not.
	// which motion should be handled, if there is many motions? -> lets consider the last element.
	result res;
	res.type = NO_TYPE;
	size_t index;
	struct group *cur;
	if (!groups.empty()) {
		for (index = groups.size()-1; index != -1; index--) {
			cur = &groups[index];
			if (cur->duration >= TIME_TO_GROUP && t >= cur->collect_time_limit) {
				// points[cur->end].x = (uint32_t) cur->x;
				// points[cur->end].y = (uint32_t) cur->y;

				// recursive call of DouglasPeucker
				marked.clear();
				marked.push_back (cur->end);
				marked.push_back (p_index-1);
				// cout << "marked size: "<<marked.size()<<endl;
				DouglasPeucker(marked, cur->end, p_index-1);

				// analyze marked.
				sort(marked.begin(), marked.end());
#ifdef DEBUG
				if (history_count != HISTORY_MAX_COUNT) {
					memset (history_vectors[history_count], 0, sizeof(size_t)*HISTORY_VECTOR_SIZE);
					memcpy (history_vectors[history_count], &marked[0], marked.size()*sizeof(size_t));
					history_ticks[history_count] = t;
					history_count++;
				}
#endif

				// printf("gesture.cpp: analyzed. size %u:\n", marked.size());
				// int i;
				// for (i = 0; i<marked.size(); i++) {
				// 	printf("[%u]: (%d, %d)\n", marked[i], points[marked[i]].x, points[marked[i]].y);
				// }

				float x[4];
				float y[4];

				size_t tmp;
				for (tmp = 0; tmp < marked.size() && tmp < 4; tmp++) {
					x[tmp] = (float) points[marked[tmp]].x;
					y[tmp] = (float) points[marked[tmp]].y;
				}
				// v case. 0.5 through 3
				cout << "analyzing... ";
				if (marked.size() >= 3 && x[1] != x[0] && x[2] != x[1]) {
					float slope1 = (y[0]-y[1])/(x[0]-x[1]);
					float slope2 = (y[1]-y[2])/(x[1]-x[2]);
					// printf("slope %f %f\n", slope1, slope2);
					if (4 > slope1 && slope1 > 0.3
						&& -4 < slope2 && slope2 < -0.3) {
						res.type = V_TYPE;
						res.V1_x = x[0];
						res.V1_y = y[0];
						res.V2_x = x[1];
						res.V2_y = y[1];
						res.V3_x = x[2];
						res.V3_y = y[2];
						cout << "v shape found" << endl;
					}
				}
				else {

				}

				// clear all preceding groups.
				for (;index != -1; index--) {
					groups.pop_front();
				}
				// cout << "size " << groups.size() << endl;
				return res;
			}
		}
	}
	return res;
}

void Gesture::visualize (Mat& img, uint32_t tick) {
	size_t i;
	for (i = 0; i < groups.size(); i++){
		// printf("G[%d] : %2d %3d,%3d ", i, groups[i].duration, (int)groups[i].x, (int)groups[i].y);
		if (groups[i].duration >= TIME_TO_GROUP) {
			// printf("go go");
			circle(img,
				Point((int)groups[i].x, (int)groups[i].y),
				8, Scalar(255, 0, 0), CV_FILLED);
			// printf("(%d, %d) ", (int)groups[i].x, (int)groups[i].y);
		}
		// printf("//");
	}
	// printf("\n");
#ifdef DEBUG
	size_t j, prev, cur;
	size_t delete_count = 0;
	uint32_t his_tick;
	Scalar color;
	for (i = 0; i < history_count; i++) {
		his_tick = history_ticks[i];
		if (tick-his_tick > HISTORY_HOLDING_TICK) {
			delete_count++;
			continue;
		}
		color = HISTORY_COLOR * (((float)(HISTORY_HOLDING_TICK-tick+his_tick))/HISTORY_HOLDING_TICK);
		
		j = 1;
		prev = history_vectors[i][0];
		while (history_vectors[i][j] != 0) {
			cur = history_vectors[i][j];
			cv::line(img,
				Point(points[prev].x,
					points[prev].y),
				Point(points[cur].x,
					points[cur].y),
				color,
				5);
			prev = cur;
			j++;
		}
	}
	memmove(history_vectors[0],
		history_vectors[delete_count],
		sizeof(size_t)*HISTORY_VECTOR_SIZE*(history_count-delete_count));
	memmove(&history_ticks[0],
		&history_ticks[delete_count],
		sizeof(size_t)*(history_count-delete_count));
	history_count -= delete_count;
#else
	// if (!marked.empty() && marked.size() > 1) {
	// 	for (i = 0; i < marked.size()-1; i++){
	// 		cv::line(img,
	// 			Point(points[marked[i]].x,
	// 				points[marked[i]].y),
	// 			Point(points[marked[i+1]].x,
	// 				points[marked[i+1]].y),
	// 			Scalar(40, 255, 125),
	// 			5);
	// 	}
	// }
#endif
}