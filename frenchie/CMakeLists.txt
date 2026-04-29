cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(Frenchie VERSION 1.0.0 LANGUAGES C CXX)

#--------------------------------------------------------------
# project options
#--------------------------------------------------------------
set(FRENCHIE_BUILD_STATIC_LIBRARY ON        CACHE BOOL   "Builds static library")
set(FRENCHIE_PLATFORM_BACKEND     "SDL3"    CACHE STRING "Use GLFW as platform backend")
set(FRENCHIE_GRAPHICS_BACKEND     "OPENGL3" CACHE STRING "Use OpenGL as rendering backend")

set(FRENCHIE_PLATFORM_BACKEND_GLFW_GIT_REPO_LINK "https://github.com/Alexey-cpu/glfw.git" CACHE STRING "The link to GLFW library repo. By default my own fork is used.")
set(FRENCHIE_PLATFORM_BACKEND_GLFW_GIT_REPO_BRANCH "origin/master" CACHE STRING "The name of branch of GLFW library repo which is used.")

set(FRENCHIE_PLATFORM_BACKEND_SDL3_GIT_REPO_LINK "https://github.com/Alexey-cpu/SDL.git" CACHE STRING "The link to SDL3 library repo. By default my own fork is used.")
set(FRENCHIE_PLATFORM_BACKEND_SDL3_GIT_REPO_BRANCH "origin/release-3.4.x" CACHE STRING "The name of branch of GLFW library repo which is used.")

if(FRENCHIE_PLATFORM_BACKEND STREQUAL "")
	set(FRENCHIE_PLATFORM_BACKEND "SDL3" CACHE STRING "" FORCE)
endif()

if(FRENCHIE_GRAPHICS_BACKEND STREQUAL "")
	set(FRENCHIE_GRAPHICS_BACKEND "OPENGL3" CACHE STRING "" FORCE)
endif()

if(FRENCHIE_BUILD_STATIC_LIBRARY)
    set(LIBRARY_TYPE "Static")
else()
    set(LIBRARY_TYPE "Shared")
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
message("-- ${PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
message("-- Build system parameters:")
message("-- Toolchain        ${CMAKE_TOOLCHAIN_FILE}")
message("-- C compiler       ${CMAKE_C_COMPILER}")
message("-- C++ compiler     ${CMAKE_CXX_COMPILER}")
message("-- CMake            ${CMAKE_VERSION}")
message("-- CMake generator  ${CMAKE_MAKE_PROGRAM}")
message("\n")
message("-- Project parameters:")
message("-- Name             ${PROJECT_NAME}")
message("-- Platform backend ${FRENCHIE_PLATFORM_BACKEND}")
message("-- Graphics backend ${FRENCHIE_GRAPHICS_BACKEND}")
message("-- Build type       ${CMAKE_BUILD_TYPE}")
message("-- Library type     ${LIBRARY_TYPE}")
message("\n")

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")

#--------------------------------------------------------------
# add library
#--------------------------------------------------------------
list(APPEND KERNEL "source/application" "source/core" "source/layers" "source/vendor")

if(FRENCHIE_PLATFORM_BACKEND STREQUAL "GLFW")
    list(APPEND KERNEL "source/backends/platform/GLFW/FrenchieApplicationPlatformBackendGLFW.hpp")
    list(APPEND KERNEL "source/backends/platform/GLFW/FrenchieApplicationPlatformBackendGLFW.cpp")
    if(FRENCHIE_GRAPHICS_BACKEND STREQUAL "OPENGL3")
        list(APPEND KERNEL "source/backends/platform/GLFW/FrenchieApplicationPlatformBackendGLFWOpenGL3.cpp")
    else()
        # TODO: support other graphics API (Vulkan, Metal, DirectX)
    endif()
elseif(FRENCHIE_PLATFORM_BACKEND STREQUAL "SDL3")
    list(APPEND KERNEL "source/backends/platform/SDL3/FrenchieApplicationPlatformBackendSDL3.hpp")
    list(APPEND KERNEL "source/backends/platform/SDL3/FrenchieApplicationPlatformBackendSDL3.cpp")
    if(FRENCHIE_GRAPHICS_BACKEND STREQUAL "OPENGL3")
        list(APPEND KERNEL "source/backends/platform/SDL3/FrenchieApplicationPlatformBackendSDL3OpenGL3.cpp")
    else()
        # TODO: support other graphics API (Vulkan, Metal, DirectX)
    endif()
endif()

if(FRENCHIE_GRAPHICS_BACKEND STREQUAL "OPENGL3")
    list(APPEND KERNEL "source/backends/rendering/OpenGL")
endif()

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

if(FRENCHIE_BUILD_STATIC_LIBRARY)
    add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})

#--------------------------------------------------------------
# configure external libraries and tools
#--------------------------------------------------------------
if(FRENCHIE_PLATFORM_BACKEND STREQUAL "GLFW")
    include("tools/glfw.cmake")
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
elseif(FRENCHIE_PLATFORM_BACKEND STREQUAL "SDL3")
    include("tools/sdl3.cmake")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
    target_include_directories(${PROJECT_NAME} PRIVATE "${sdl3_SOURCE_DIR}/include/")
endif()

if(FRENCHIE_GRAPHICS_BACKEND STREQUAL "OPENGL3")
    target_include_directories(${PROJECT_NAME} PRIVATE "source/backends/rendering/OpenGL/glad/include/")
endif()

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)