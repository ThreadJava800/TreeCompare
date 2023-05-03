#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define mmax(val1, val2) {               \
    return val1 > val2 ? val1 : val2;     \
}                                          \


#define mmin(val1, val2) {               \
    return val1 < val2 ? val1 : val2;     \
}                                          \

#if MAKE_CHECKS
#define ON_ERROR(expr, errStr, retVal) {                 \
    if (expr) {                                           \
        fprintf(stderr, "FATAL ERROR: %s\n", errStr);      \
        return retVal;                                      \
    }                                                        \
}                                                             \

#else
#define ON_ERROR(expr, errStr, retVal) {}
#endif