#include "timeevents.h"
#include "FreeRTOS.h"



static /*volatile*/ time_t unix_time = 1442466000;  //Sun, 20 Sep 2015 05:00:00 GMT
struct tm *tm_local;

/*volatile const*/  time_t* TimeEvent_GetUnixTime(void)     {       return &unix_time;      }


                                  //Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec
static const uint8_t monthDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int GetDayCntOfMounth(int month, int year)
{
  if( month == 1 ) {
    if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) )       /* високосный если - кратен 4 и не кратен 100     */ 
                                                                            /*                 - кратен 400                   */
    return monthDay[month] + 1;
  }
  return monthDay[month];
}
int GetCountDayYear(int year)
{
  int dday;
  
  dday=0;
  for(int i=0; i<12; i++)
    dday += GetDayCntOfMounth(i, year);
  return dday;
}

void NextMonthStart(struct tm *time)
{
  int DayRest = GetDayCntOfMounth(time->tm_mon, 1900 + time->tm_year) - time->tm_mday;
  DayRest+=2;
  time->tm_mday = 1;
  
  time->tm_mon++;
  if(time->tm_mon > 11) {
    time->tm_mon = 0;
    time->tm_year++;
  }
  time->tm_wday +=  DayRest;
  time->tm_wday %= 7;
  
  time->tm_yday += DayRest;
  if( time->tm_yday > GetCountDayYear(1900 + time->tm_year) )
    time->tm_yday %= GetCountDayYear(1900 + time->tm_year);  
}

void NextDayStart(struct tm *time)
{
  time->tm_mday++;
  if( time->tm_mday > GetDayCntOfMounth( time->tm_mon, 1900 + time->tm_year ) ) {
    time->tm_mday = 0;
    NextMonthStart( time );
  }
  time->tm_wday++;
  if(time->tm_wday > 6 ) time->tm_wday = 0;
  
  time->tm_yday++;
  if( time->tm_yday > GetCountDayYear(1900 + time->tm_year) )
    time->tm_yday = 0;
}
void NextHourStart(struct tm *time)
{
  time->tm_hour++;
  if(time->tm_hour > 23 ) {
    time->tm_hour = 0;
    NextDayStart( time );
  }
}


time_t GetStartNextHour(time_t time)
{
  struct tm *t_struct;
  
  t_struct = localtime(&time);
  t_struct->tm_min = t_struct->tm_sec = 0;
  NextHourStart( t_struct );
  return mktime(t_struct);
}

time_t GetStartNextHalfHour(time_t time)
{
  struct tm *t_struct;
  
  t_struct = localtime(&time);
  t_struct->tm_sec = 0;
    
  if(t_struct->tm_min >= 30 ) {
    t_struct->tm_min = 0;
    NextHourStart( t_struct );
  }else {
    t_struct->tm_min = 30;
  }
  return mktime( t_struct );
}

// Returns seconds since day start
time_t GetStartCurrenttDay(time_t time)
{
  struct tm *tm_day;
  
  tm_day = localtime(&time);
  tm_day->tm_hour = tm_day->tm_min = tm_day->tm_sec = 0;
  return mktime(tm_day);
}

time_t GetStartNextDay(time_t time)
{
  struct tm *t_struct;
  
  t_struct = localtime(&time);
  t_struct->tm_hour = t_struct->tm_min = t_struct->tm_sec = 0;
  NextDayStart( t_struct );
  return mktime( t_struct );
}

time_t GetStartNextMonth(time_t time)
{
  struct tm *t_struct;
  
  t_struct = localtime(&time);
  t_struct->tm_hour = t_struct->tm_min = t_struct->tm_sec = 0;
  NextMonthStart( t_struct );
  return mktime( t_struct );
}

