#ifndef DEFINES_HW_H
#define DEFINES_HW_H

#ifdef VER_2_1
#include "defines_VER_100.h"
#endif

#ifdef VER_2_3
#include "defines_VER_103.h"
#endif

#ifdef VER_3_0
#include "defines_VER_300.h"
#endif

#ifdef VER_SPLIT
#include "defines_VER_110_split.h"
#endif

#include "ade7953.h"
#include "ade7868a.h"

#endif
