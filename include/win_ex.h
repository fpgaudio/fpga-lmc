/**
 * This header defines useful utility fnuctions.
 */

#ifndef WIN_EX_H
#define WIN_EX_H

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

char* win32_err_str(void);
char* current_time_str(void);

#endif