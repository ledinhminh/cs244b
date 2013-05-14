#include "stdio.h"
#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define PRINT(...) \
    do { if (DEBUG_TEST) fprintf(stderr, __VA_ARGS__); } while (0)
