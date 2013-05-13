#include "english_time.h"
#include "string.h"

static const char* const ONETEENS[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen",
  "fourteen",
  "fifteen",
  "sixteen",
  "seventeen",
  "eighteen",
  "nineteen"
};

static const char* const TWENS[] = {
  "twenty",
  "thirty",
  "forty",
  "fifty",
};

static const char* STR_TEEN = "teen";
static const char* STR_OH_CLOCK = "o'clock";
static const char* STR_OH = "o'";
static const char* STR_HUNDRED = "hundred";

void english_time_2lines(int hours, int minutes, char* str_hour, char* str_minute) {
    
  strcpy(str_hour, "");
  strcpy(str_minute, "");

  if(hours == 0) {
    strcat(str_hour,"twelve");
  }
  else {
    if (hours > 12) {
      hours = hours - 12;
    }
    strcat(str_hour, ONETEENS[hours]);
  }

  if(minutes == 0){
    strcat(str_minute,STR_OH_CLOCK);
  }
  else {
    if(minutes < 20) {
      if(minutes < 10) {
        strcat(str_minute, STR_OH);  
      }
      strcat(str_minute, ONETEENS[minutes]);
      if((minutes == 14) || (minutes > 15)) {
        strcpy(str_minute, ONETEENS[minutes-10]);
        strcat(str_minute, "\n");
        strcat(str_minute, STR_TEEN);
      }
    }
    else {
      strcat(str_minute, TWENS[((minutes/10)%10)-2]);
      if((minutes%10)>0) {
        strcat(str_minute, "\n");
        strcat(str_minute, ONETEENS[minutes%10]); 
      } 
    }
  }
}

void english_time_3lines(int hours, int minutes, char* str_hour, char* str_minute1, char* str_minute2) {
  
  strcpy(str_hour, "");
  strcpy(str_minute1, "");
  strcpy(str_minute2, "");

  if(hours == 0) {
    strcat(str_hour,"twelve");
  }
  else {
    if (hours > 12) {
      hours = hours - 12;
    }
    strcat(str_hour, ONETEENS[hours]);
  }

  if(minutes == 0){
    strcat(str_minute1,STR_OH_CLOCK);
  }
  else {
    if(minutes < 20) {
      if(minutes < 10) {
        strcat(str_minute1, STR_OH);  
      }
      strcat(str_minute1, ONETEENS[minutes]);
      if((minutes == 14) || (minutes > 15)) {
        strcpy(str_minute1, ONETEENS[minutes-10]);
        strcpy(str_minute2, STR_TEEN);
      }
    }
    else {
      strcat(str_minute1, TWENS[((minutes/10)%10)-2]);
      if((minutes%10)>0) {
        strcat(str_minute2, ONETEENS[minutes%10]); 
      } 
    }
  }
}

void military_time_4lines(int hours, int minutes, char* str_hour1, char* str_hour2, char* str_minute1, char* str_minute2){

  strcpy(str_hour1, "");
  strcpy(str_hour2, "");
  strcpy(str_minute1, "");
  strcpy(str_minute2, "");

  if((hours == 14) || (hours > 15)) {
    strcpy(str_hour1, ONETEENS[hours-10]);
    strcpy(str_hour2, STR_TEEN);
  }
  else {
    strcpy(str_hour2, ONETEENS[hours]);
  }

  if(minutes == 0){
    strcpy(str_minute1,STR_HUNDRED);
  }
  else {
    if(minutes < 20) {
      if(minutes < 10) {
        strcpy(str_minute1, ONETEENS[0]);
        strcpy(str_minute2, ONETEENS[minutes]);
      }
      else if ((minutes == 14) || (minutes > 15)) {
        strcpy(str_minute1, ONETEENS[minutes-10]);
        strcpy(str_minute2, STR_TEEN);
      }
      else {
        strcpy(str_minute1, ONETEENS[minutes]);
      }
    }
    else {
      strcpy(str_minute1, TWENS[((minutes/10)%10)-2]);
      if((minutes%10)>0) {
        strcpy(str_minute2, ONETEENS[minutes%10]); 
      } 
    }
  }

}
