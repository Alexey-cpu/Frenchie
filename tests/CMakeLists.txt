#--------------------------------------------------------------
# setup CMake
#--------------------------------------------------------------
cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(FrenchieGUIExampleProject VERSION 1.0.0 LANGUAGES C CXX)

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
# execute tests tests
#--------------------------------------------------------------
enable_testing()

add_subdirectory("FrenchieMathTestProject/" math_test)
add_subdirectory("FrenchieGUITestProject/" gui_test)

add_test(NAME MATH COMMAND FrenchieMathTestProject)
add_test(NAME GUI COMMAND FrenchieGUITestProject "test")