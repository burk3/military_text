#pragma once

#include "string.h"

void english_time_2lines(int hours, int minutes, char* str_hour, char* str_minute);
void english_time_3lines(int hours, int minutes, char* str_hour, char* str_minute1, char* str_minute2);
void military_time_4lines(int hours, int minutes, char* str_hour1, char* str_hour2, char* str_minute1, char* str_minute2);