cmake_minimum_required(VERSION 3.10.0)
project(OpenGL VERSION 0.1.0 LANGUAGES C CXX)

#==============================================================================================================
# setup common project options
#==============================================================================================================
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# message("===================================================")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O3")
# set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} /O3")
# message(${CMAKE_CXX_FLAGS})
# message("===================================================")

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")
include("tools/fmt.cmake")
include("tools/glfw.cmake")
include("tools/glm.cmake")
include("tools/spdlog.cmake")
include("tools/rapidjson.cmake")

add_subdirectory("tools/pugixml/")

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
list(APPEND KERNEL "source/")
set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(
    OpenGL
    
    # ImGUI
    tools/imgui/imconfig.h
    tools/imgui/imgui.h
    tools/imgui/imgui_internal.h
    tools/imgui/imgui.cpp
    tools/imgui/imgui_demo.cpp
    tools/imgui/imgui_draw.cpp
    tools/imgui/imgui_tables.cpp
    tools/imgui/imgui_widgets.cpp

    # ImGUI GLFW backend
    tools/imgui/backends/imgui_impl_glfw.h
    tools/imgui/backends/imgui_impl_glfw.cpp

    # ImGUI OpenGL3 backend
    tools/imgui/backends/imgui_impl_opengl3.h
    tools/imgui/backends/imgui_impl_opengl3.cpp

    ${HEADERS} 
    ${SOURCES}
    )
target_link_libraries(OpenGL glfw glm::glm fmt::fmt spdlog::spdlog pugixml::pugixml)

target_include_directories(
    OpenGL 
    PUBLIC ${DIRECTORIES} 
    
    #GLAD
    "source/vendor/glad/include/"

    #IMGUI
    "tools/imgui/"
    "tools/imgui/backends/"
    "tools/glad/include/"
    
    #RAPIDJSON
    "tools/rapidjson/include"
    )

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
