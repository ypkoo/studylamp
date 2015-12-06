#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/time.h>
#endif

unsigned int getTick(void)
{
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