#include "Logger.h"

#ifdef _WIN32
#include <Windows.h>
#endif

void Logger::Init()
{
    // Allows for colored text through escape sequences on Windows 10 and 11
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hConsole, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, dwMode);
#endif
}
