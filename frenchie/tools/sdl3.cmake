include(FetchContent)

# load SDL from remote
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/Alexey-cpu/SDL.git
    GIT_TAG origin/release-3.4.x
    OVERRIDE_FIND_PACKAGE)

# configure SDL
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)