include(FetchContent)

# load SDL from remote
FetchContent_Declare(
    GLFW
    GIT_REPOSITORY https://github.com/Alexey-cpu/glfw.git
    GIT_TAG origin/master
    OVERRIDE_FIND_PACKAGE)

# configure GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_LIBRARY_TYPE "STATIC" CACHE STRING "Library type override for GLFW (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)" FORCE)

FetchContent_MakeAvailable(GLFW)