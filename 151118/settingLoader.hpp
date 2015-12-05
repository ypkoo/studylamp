// settingLoader.hpp
// 
// Provide convenience to reduce efforts to compile.
//
/// 1. The syntax of each line of Settings.txt:
// LINE = NOTHING
//        | #(any comments)
//        | (name-without-space)[ \t]*(decimal-number)
//        | (name-without-space)[ \t]*f(float-number)
// The empty space between name and decimal number could be [ \t]*
//
// 2. Use of this module
// Call setting_init() for the first time.
// Call setting_load_u32(SETTING_NAME, DEFAULT_VALUE) for each SETTING_NAME.
// ,or call setting_load_float(SETTING_NAME, DEFAULT_VALUE) for each SETTING_NAME.
//
// @author S.H.Lee
// 
// @version 1.0
// @since 2015-12-05
// First implementation

#ifndef _SETTING_LOADER_H_
#define _SETTING_LOADER_H_

#include <stdint.h>

#define SETTING_COUNT_MAX 100
#define SETTING_NAME_SIZE_MAX 40
#define SETTING_LINE_SIZE_MAX 200

namespace setting {
	enum _SETTING_TYPE{
		SETTING_INTEGER_TYPE,
		SETTING_FLOAT_TYPE
	};
	struct setting_s {
		char name[SETTING_NAME_SIZE_MAX + 1];
		uint32_t value;
		float value_f;
		enum _SETTING_TYPE type;
	};
}


void setting_init (void);
uint32_t setting_load_u32 (const char *name, int default_val);
float setting_load_float (const char *name, float default_val);

#endif
