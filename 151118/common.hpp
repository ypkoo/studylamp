#ifndef _COMMON_HPP_
#define _COMMON_HPP_


enum PROGRAM_STATUS {
    STATUS_BOOKCOVER      = 0,
    STATUS_MAINMENU       = 1,
    STATUS_STUDY_LEARNING = 2,
    STATUS_STUDY_SOLVING  = 3,
    STATUS_STUDY_SOLVED   = 4,
    STATUS_PROGRESS       = 5,
    STATUS_REVIEW         = 6,
    STATUS_BUFFER         = 7,
    STATUS_MOVIE_RUNNING  = 8, // From state 2, added
};
#define STATUS_MAX 8
#define NUMBER_OF_STATUS STATUS_MAX+2

// Get current time tick, with unit of milliseconds.
unsigned int getTick(void);

#endif
