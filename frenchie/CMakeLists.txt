cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(Frenchie VERSION 0.1.0 LANGUAGES C CXX)

#--------------------------------------------------------------
# project options
#--------------------------------------------------------------
option(PLATFORM_BACKEND_GLFW   "Use GLFW as platform backend"    OFF)
option(PLATFORM_BACKEND_SDL    "Use SDL as platform backend"     OFF)
option(GRAPHICS_BACKEND_OPENGL "Use OpenGL as rendering backend" ON)

set(PLATFORM_BACKEND_GLFW OFF CACHE BOOL "Use GLFW" FORCE)
set(PLATFORM_BACKEND_SDL ON CACHE BOOL "Use SDL3" FORCE)

if(PLATFORM_BACKEND_GLFW)
    set(PLATFORM_BACKEND "GLFW")
    set(PLATFORM_BACKEND_SDL OFF CACHE BOOL "" FORCE)
endif()

if(PLATFORM_BACKEND_SDL)
    set(PLATFORM_BACKEND "SDL")
    set(PLATFORM_BACKEND_GLFW OFF CACHE BOOL "" FORCE)
endif()

if(GRAPHICS_BACKEND_OPENGL)
    set(GRAPHICS_BACKEND "OpenGL")
endif()

#--------------------------------------------------------------
# setup common project options
#--------------------------------------------------------------
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

message("\n")
message("-- Build system parameters:")
message("-- Toolchain        ${CMAKE_TOOLCHAIN_FILE}")
message("-- C compiler       ${CMAKE_C_COMPILER}")
message("-- C++ compiler     ${CMAKE_CXX_COMPILER}")
message("-- CMake            ${CMAKE_VERSION}")
message("-- CMake generator  ${CMAKE_MAKE_PROGRAM}")
message("\n")
message("-- Project parameters:")
message("-- Name             ${PROJECT_NAME}")
message("-- Platform backend ${PLATFORM_BACKEND}")
message("-- Graphics backend ${GRAPHICS_BACKEND}")
message("-- Build type       ${CMAKE_BUILD_TYPE}")
message("\n")

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")

#--------------------------------------------------------------
# add library
#--------------------------------------------------------------
list(APPEND KERNEL "source/core" "source/layers" "source/vendor" "source/application" "source/sandbox")

if(PLATFORM_BACKEND_GLFW)
    list(APPEND KERNEL "source/backends/GLFW/FrenchieApplicationPlatformBackendGLFW.cpp")
elseif(PLATFORM_BACKEND_SDL)
    if(GRAPHICS_BACKEND_OPENGL)
        list(APPEND KERNEL "source/backends/SDL3/FrenchieApplicationPlatformBackendSDL3OpenGL.cpp")
    else()
        # TODO: support other graphics API (Vulkan, Metal, DirectX)
    endif()
endif()

if(GRAPHICS_BACKEND_OPENGL)
    list(APPEND KERNEL "source/backends/OpenGL")
endif()

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

#add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "source/main.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})

#--------------------------------------------------------------
# configure external libraries and tools
#--------------------------------------------------------------
if(PLATFORM_BACKEND_GLFW)
    include("tools/glfw.cmake")
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
elseif(PLATFORM_BACKEND_SDL)
    include("tools/sdl3.cmake")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
    target_include_directories(${PROJECT_NAME} PRIVATE "${SDL3_SOURCE_DIR}/include/")
endif()

if(GRAPHICS_BACKEND_OPENGL)
    target_include_directories(${PROJECT_NAME} PRIVATE "source/backends/OpenGL/glad/include/")
endif()

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)