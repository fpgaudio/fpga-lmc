#include "win_ex.h"
#include <time.h>
#include <Windows.h>
#include <math.h>

static char s_err_msg_buf[1024 * 10];

char* win32_err_str()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0) {
        return NULL;
    }

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    (void)FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&s_err_msg_buf, sizeof(s_err_msg_buf) / sizeof(TCHAR),
        NULL
    );

    return s_err_msg_buf;
}

char* current_time_str() {
    time_t rawTime;
    struct tm* currentTime;

    time(&rawTime);
    currentTime = localtime(&rawTime);

    // By default this returns a C string with a trailing newline that we
    // don't want.
    char* strTime = asctime(currentTime);
    strTime[strlen(strTime) - 1] = 0;

    return strTime;
}

#define FLOAT_FP_CONVERSION_FACTOR (16)

int32_t float2i32(float in) {
    return (int32_t)(round(in * (1 << FLOAT_FP_CONVERSION_FACTOR)));
}

float i322float(int32_t in) {
    return (float)in / powf(2, FLOAT_FP_CONVERSION_FACTOR);
}

float bound_linear_normalize(float in, float coeff_k, float coeff_c, float minv, float maxv) {
    return min(maxv, max(minv, coeff_k * in + coeff_c));
}