#include "win_ex.h"
#include <time.h>

#include <Windows.h>

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