#pragma once

#if defined(unix) || defined(__unix) || defined(__unix__)
#define IS_LINUX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define IS_MACOS
#endif

#if defined(_WIN32) || defined(_WIN64)
#define IS_WINDOWS
#endif

#if defined(IS_LINUX) || defined(IS_MACOS)
#define IS_UNIX
#endif