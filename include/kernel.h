#pragma once

#include <inttypes.h>

#if defined(__cplusplus)
extern "C"  {
#endif

void die(const char *msg);

void die_extra(const char *msg, const char *extra);

#if defined(__cplusplus)
}
#endif
