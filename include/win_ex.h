/**
 * This header defines useful utility fnuctions.
 */

#ifndef WIN_EX_H
#define WIN_EX_H

#include <stdint.h>

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

char* win32_err_str(void);
char* current_time_str(void);
int32_t float2i32(float in);
float i322float(int32_t in);
float bound_linear_normalize(float in, float coeff_k, float coeff_c, float minv, float maxv);

#endif