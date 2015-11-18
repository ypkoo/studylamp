#ifndef GESTURE_H
#define GESTURE_H
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdint.h>

namespace gest{
	const size_t POINT_COUNT = 1000000;
	const size_t CLUSTER_COUNT = 100;

	const uint32_t SIZE_LIMIT_SQUARE = 400;
	const uint32_t TIME_LIMIT = 800;
	const uint32_t TIME_TO_CLUSTER = 500;

	struct tp{
		uint32_t x;
		uint32_t y;
		uint32_t t; // milliseconds
	};

	struct cluster{
		float cx;
		float cy;
		uint32_t duration;

		uint32_t start;
		uint32_t end;
		uint32_t count; // same with end - start + 1
	};

	struct line{
		float a;
		float b;
		float c;

		float slope;
		float length;
		float duration;

		bool is_linear;
	};
}

using namespace cv;
using namespace gest;

class gesture
{
private:
	struct tp *points;
	struct cluster *clusters;
	struct line *lines;

	size_t pindex;
	size_t cindex;
public:
	gesture();
	void registerPoint (uint32_t x, uint32_t y, uint32_t t);
	void clear (void);
	void updateLine (size_t index);

	void check (void);
	void visualize (Mat& img);
};

#endif