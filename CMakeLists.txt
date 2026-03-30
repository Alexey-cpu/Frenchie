cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(Frenchie VERSION 0.1.0 LANGUAGES C CXX)

#--------------------------------------------------------------
# project options
#--------------------------------------------------------------
option(PLATFORM_BACKEND_GLFW "Use GLFW as platform backend" ON)
option(RENDERING_BACKEND_OPENGL "Use OpenGL as rendering backend" ON)

#--------------------------------------------------------------
# setup common project options
#--------------------------------------------------------------
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

message("C compiler: ${CMAKE_C_COMPILER}")
message("C++ compiler: ${CMAKE_CXX_COMPILER}")
message("CMake: ${CMAKE_VERSION}")
message("CMake generator: ${CMAKE_MAKE_PROGRAM}")

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
list(APPEND KERNEL "source/core" "source/layers" "source/vendor" "source/application" "source/sandbox")

if(PLATFORM_BACKEND_GLFW)
    include("tools/glfw.cmake")
    list(APPEND KERNEL "source/backends/GLFW")
endif()

if(RENDERING_BACKEND_OPENGL)
    list(APPEND KERNEL "source/backends/OpenGL")
endif()

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "source/main.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})

#--------------------------------------------------------------
# configure external libraries and tools
#--------------------------------------------------------------
if(PLATFORM_BACKEND_GLFW)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
endif()

if(RENDERING_BACKEND_OPENGL)
    target_include_directories(${PROJECT_NAME} PRIVATE "source/backends/OpenGL/glad/include/")
endif()

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
