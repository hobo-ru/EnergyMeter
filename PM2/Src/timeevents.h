#ifndef TIMEEVENTS_H
#define TIMEEVENTS_H

#include <time.h>

extern time_t GetStartNextMonth(time_t time);
extern time_t GetStartNextDay(time_t time);
extern time_t GetStartNextHour(time_t time);
extern time_t GetStartNextHalfHour(time_t time);
extern time_t GetStartCurrenttDay(time_t time);

#endif
