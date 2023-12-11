#pragma once
#include <array>
#include <cstdarg>
#include <cstring>
#include <mutex>

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

#ifndef NDEBUG
#    define debugLog logs::Write
#    define debugInit logs::Init
#else
#    define debugLog(messTmp, params...)
#    define debugInit(logFile)
#endif

namespace logs {
void Init(char* fName);
void Write(const char* Format, ...);
}    // namespace logs
