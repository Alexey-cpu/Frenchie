cmake_minimum_required(VERSION 3.10.0)
project(OpenGL VERSION 0.1.0 LANGUAGES C CXX)

#==============================================================================================================
# setup common project options
#==============================================================================================================
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")
include("tools/fmt.cmake")
include("tools/glfw.cmake")
include("tools/glm.cmake")
include("tools/spdlog.cmake")

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
list(APPEND KERNEL "source/")
set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

add_executable(OpenGL ${HEADERS} ${SOURCES})
target_link_libraries(OpenGL glfw glm::glm fmt::fmt spdlog::spdlog)

target_include_directories(OpenGL PUBLIC ${DIRECTORIES} "source/opengl/glad/include/")

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
