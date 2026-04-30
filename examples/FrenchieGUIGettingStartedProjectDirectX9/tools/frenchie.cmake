include(FetchContent)

# load Frenchie from remote
FetchContent_Declare(
    Frenchie
    GIT_REPOSITORY "https://github.com/Alexey-cpu/Frenchie.git"
    GIT_TAG "origin/v1/release"
    OVERRIDE_FIND_PACKAGE
	GIT_SHALLOW TRUE)

# configure Frenchie
set(FRENCHIE_BUILD_STATIC_LIBRARY ON        CACHE BOOL   "Set libary type" FORCE)
set(FRENCHIE_PLATFORM_BACKEND     "SDL3"    CACHE STRING "Set platform backend" FORCE)
set(FRENCHIE_GRAPHICS_BACKEND     "OPENGL3" CACHE STRING "Set rendering backend" FORCE)

FetchContent_MakeAvailable(Frenchie)