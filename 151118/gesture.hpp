// gesture.hpp
// 
// Header file for Gesture class
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

#ifndef _GESTURE_HPP_
#define _GESTURE_HPP_
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdint.h>
#include <deque>

using namespace cv;

namespace gesture{
	const size_t POINT_COUNT = 1000000;
	// const size_t GROUP_COUNT = 100;

	// const uint32_t NEW_GROUP_TIME = 800; // Is it really needed?
	const uint32_t TIME_TO_GROUP = 800; // the group object is complete or not
	const uint32_t TIME_TO_MOTION = 1000;

	struct timePoint{
		uint32_t x;
		uint32_t y;
		uint32_t t; // milliseconds
	};

	struct group{
		float x;
		float y;
		uint32_t duration; // if this is bigger than TIME_TO_GROUP, it means complete group.

		size_t start;
		size_t end;
		size_t count; // same with end-start+1

		uint32_t collect_time_limit; // only valid for completed group.
	};

	enum result_type {
		NO_TYPE,
		V_TYPE,
		LINELINE_TYPE
	};
	struct result {
		enum result_type type;
		union {
			struct {
				uint32_t V1_x;
				uint32_t V1_y;
				uint32_t V2_x;
				uint32_t V2_y;
				uint32_t V3_x;
				uint32_t V3_y;
			};
			struct {
				uint32_t L1_x;
				uint32_t L1_y;
				uint32_t L2_x;
				uint32_t L2_y;
			};
		};
	};
}

class Gesture
{
private:
	struct gesture::timePoint *points;
	size_t p_index;
	std::deque<struct gesture::group> groups;
	std::vector<size_t> marked;

	uint32_t GROUP_SIZE_LIMIT_SQUARE;
	uint32_t GROUP_DETAILED_SIZE_LIMIT_SQUARE;
	float EPSILON;

	void DouglasPecker(std::vector<size_t>&, size_t, size_t);
public:
	Gesture(uint32_t width, uint32_t height);
	~Gesture();
	gesture::result registerPoint (int32_t x, int32_t y, uint32_t t);
	void visualize (Mat& img);
};

#endif