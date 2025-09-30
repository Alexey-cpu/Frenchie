set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "Build 8 bit PCRE2 library" FORCE)
set(PCRE2_BUILD_PCRE2_16 ON CACHE BOOL "Build 16 bit PCRE2 library" FORCE)
set(PCRE2_BUILD_PCRE2_32 ON CACHE BOOL "Build 32 bit PCRE2 library" FORCE)
add_subdirectory("tools/pcre2/")