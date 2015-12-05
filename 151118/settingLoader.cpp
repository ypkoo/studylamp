// settingLoader.cpp
// 
// Provide convenience to reduce efforts to compile.
//
// 1. The syntax of each line of Settings.txt:
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

#include "settingLoader.hpp"

#include <fstream>
#include <string.h>
#include <cstring>
#include <stdlib.h>

using namespace std;
using namespace setting;

static const char setting_file[20] = "Settings.txt";
static struct setting_s settings[SETTING_COUNT_MAX];
static size_t _count;

char *
strtok_r (char *s, const char *delimiters, char **save_ptr) 
{
	char *token;

	if (s == NULL)
	s = *save_ptr;

	while (strchr (delimiters, *s) != NULL) 
	{
		if (*s == '\0')
		{
			*save_ptr = s;
			return NULL;
		}

		s++;
	}

	token = s;
	while (strchr (delimiters, *s) == NULL)
		s++;
	if (*s != '\0') 
	{
		*s = '\0';
		*save_ptr = s + 1;
	}
	else 
		*save_ptr = s;
	return token;
}

void setting_init (){
	ifstream input(setting_file);
	char buf[SETTING_LINE_SIZE_MAX];

	char *tmp, *save_ptr;

	_count = 0;
	uint32_t line = -1; // for debugging purpose.
	while (!input.eof()) {
		line++;
		input.getline(buf, SETTING_LINE_SIZE_MAX);

		// read name
		tmp = strtok_r (buf, " \t\n\r", &save_ptr);
		if (tmp == NULL || tmp[0] == '#') // comments
			continue;
		strcpy (settings[_count].name, tmp);

		// read number
		tmp = strtok_r (NULL, " \t\n\r", &save_ptr);
		if (tmp == NULL) {
			fprintf (stderr, "setting_init(): read line %d error with code 0\n", line);
			exit(-1);
		}
		if (tmp[0] == 'f'){
			sscanf(tmp+1, "%f", &settings[_count].value_f);
			settings[_count].type = SETTING_FLOAT_TYPE;
		}
		else{
			sscanf(tmp, "%d", &settings[_count].value);
			settings[_count].type = SETTING_INTEGER_TYPE;
		}


		_count++;
	}
	input.close();
}

uint32_t setting_load_u32 (const char *name, int default_val){
	int i;
	for (i=0; i<_count; i++)
		if (strcmp(settings[i].name, name) == 0
			&& settings[i].type == SETTING_INTEGER_TYPE)
			return settings[i].value;
	return default_val;
}

float setting_load_float (const char *name, float default_val){
	int i;
	for (i=0; i<_count; i++)
		if (strcmp(settings[i].name, name) == 0
			&& settings[i].type == SETTING_FLOAT_TYPE)
			return settings[i].value_f;
	return default_val;
}

