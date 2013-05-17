#ifndef DEBUG_H
#define DEBUG_H

#include "stdio.h"
//#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define PRINT(...) \
    do { if (DEBUG_TEST) fprintf(stdout, __VA_ARGS__); } while (0)

#endif
