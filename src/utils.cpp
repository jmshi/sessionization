#include <iostream>
#include <string>
#include <time.h>
//
// utility functions
//
// convert string "2000-01-01 00:00:00"
// to time_t in seconds
time_t str_to_seconds(std::string starttime)
{
  int yy,mn,dd,hh,mm,ss;
  struct tm tms;
  const char *tstring = starttime.c_str();

  sscanf(tstring,"%d-%d-%d %d:%d:%d",&yy,&mn,&dd,&hh,&mm,&ss);
  tms.tm_year = yy - 1900;
  tms.tm_mon = mn - 1;
  tms.tm_mday = dd;
  tms.tm_hour = hh;
  tms.tm_min = mm;
  tms.tm_sec = ss;
  tms.tm_isdst = -1;

  return mktime(&tms);
}

//convert time_t seconds to YYYY-MM-DD HH:MM:SS format string
std::string seconds_to_str(time_t seconds)
{
  char buff[22];
  strftime(buff,22,"%Y-%m-%d %H:%M:%S",localtime(&seconds));
  std::string str(buff);
  return str;
}
