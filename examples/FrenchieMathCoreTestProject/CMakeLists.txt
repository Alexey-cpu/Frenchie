cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(FrenchieMathCoreTestProject VERSION 1.0.0 LANGUAGES C CXX)

#--------------------------------------------------------------
# setup common project options
#--------------------------------------------------------------
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
# this is a really handy macro that collects
# source code within predefined list of directories
include("${CMAKE_SOURCE_DIR}/../../frenchie/cmake/collect_source_code_and_resources.cmake")
include("tools/glm.cmake")

list(APPEND KERNEL "source/" "${CMAKE_SOURCE_DIR}/../../frenchie/source/core/")
list(APPEND KERNEL "source/" "${CMAKE_SOURCE_DIR}/../../frenchie/source/vendor/")

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)