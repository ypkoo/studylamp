// gesture.cpp
// 
// Gesture class
// Analyze continuous points set
// 
// @author S.H.Lee
// 
// @version 1.1
// @since 2015-11-23
// Modified code for more natural recognition.
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

Gesture::Gesture() : groups()
{
	points = (struct timePoint *) malloc(sizeof(struct timePoint) * POINT_COUNT);
	p_index = 0;
}

Gesture::~Gesture()
{
}
void Gesture::DouglasPecker(std::vector<size_t>& _marked, size_t front, size_t end){
	float dmax = 0; size_t index = 0;
	size_t i;
	float d;

	// equation of line L:Ax+By+C=0, which passes through points[front] and point[end]
	int32_t A = points[end].y - points[front].y;
	int32_t B = points[front].x - points[end].x;
	int32_t C = points[end].x*points[front].y - points[front].x*points[end].y;
	float sqABsq = sqrt(A*A+B*B);

	int32_t x, y;
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

	if (dmax > epsilon) {
		if (index != front && index != end)
			_marked.push_back(index);
		if (index - front > 1)
			DouglasPecker(_marked, front, index);
		if (end - index > 1)
			DouglasPecker(_marked, index, end);
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
			if (xdiff * xdiff + ydiff * ydiff < SIZE_LIMIT_SQUARE) {
				end_gp->x = ((end_gp->x*count)+x)/(count+1);
				end_gp->y = ((end_gp->y*count)+y)/(count+1);
				end_gp->duration +=  t-points[p_index-1].t;
				end_gp->count++;
				end_gp->collect_time_limit = t + TIME_TO_MOTION;
			} else { // so far
				// if group is not completed, delete and make new group.
				if (end_gp->duration < TIME_TO_GROUP) {
					end_gp->x = x;
					end_gp->y = y;
					end_gp->duration = 0;
					end_gp->start = end_gp->end = p_index;
					end_gp->count = 1;
					end_gp->collect_time_limit = 0; // it is not meaningful to set.
				}
				// else make new group.
				else {
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
			if (cur->duration > TIME_TO_GROUP && t >= cur->collect_time_limit) {
				// points[cur->end].x = (uint32_t) cur->x;
				// points[cur->end].y = (uint32_t) cur->y;

				// recursive call of DouglasPecker
				marked.clear();
				marked.push_back (cur->end);
				marked.push_back (p_index-1);
				// cout << "marked size: "<<marked.size()<<endl;
				DouglasPecker(marked, cur->end, p_index-1);

				// analyze marked.
				sort(marked.begin(), marked.end());

				printf("gesture.cpp: analyzed. size %u:\n", marked.size());
				int i;
				for (i = 0; i<marked.size(); i++) {
					printf("[%u]: (%d, %d)\n", marked[i], points[marked[i]].x, points[marked[i]].y);
				}

				float x[4];
				float y[4];

				size_t tmp;
				for (tmp = 0; tmp < marked.size() && tmp < 4; tmp++) {
					x[tmp] = (float) points[marked[tmp]].x;
					y[tmp] = (float) points[marked[tmp]].y;
				}
				// v case. 0.5 through 3
				if (marked.size() >= 3 && x[1] != x[0] && x[2] != x[1]) {
					float slope1 = (y[0]-y[1])/(x[0]-x[1]);
					float slope2 = (y[1]-y[2])/(x[1]-x[2]);
					if (-0.5 > slope1 && slope1 > -3
						&& 0.5 < slope2 && slope2 < 3) {
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

void Gesture::visualize (Mat& img) {
	size_t i;
	for (i = 0; i < groups.size(); i++){
		if (groups[i].duration > TIME_TO_GROUP) {
			circle(img,
				Point(groups[i].x, groups[i].y),
				8, Scalar(255, 0, 0), CV_FILLED);
			// printf("(%d, %d) ", (int)groups[i].x, (int)groups[i].y);
		}
	}
	if (!marked.empty() && marked.size() > 1) {
		for (i = 0; i < marked.size()-1; i++){
			cv::line(img,
				Point(points[marked[i]].x,
					points[marked[i]].y),
				Point(points[marked[i+1]].x,
					points[marked[i+1]].y),
				Scalar(40, 255, 40),
				5);
		}
	}
		
}