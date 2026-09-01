#pragma once

#if defined(unix) || defined(__unix) || defined(__unix__)
#define FRENCHIE_APPLICATION_PLATFORM_IS_LINUX
#endif

#if defined(__APPLE__) || defined(__MACH__)
#define FRENCHIE_APPLICATION_PLATFORM_IS_MACOS
#endif

#if defined(_WIN32) || defined(_WIN64)
#define FRENCHIE_APPLICATION_PLATFORM_IS_WINDOWS
#endif

#if defined(FRENCHIE_APPLICATION_PLATFORM_IS_LINUX) || defined(FRENCHIE_APPLICATION_PLATFORM_IS_MACOS)
#define FRENCHIE_APPLICATION_PLATFORM_IS_UNIX
#endif