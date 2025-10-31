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

message("===================================================")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O3")
# set(CMAKE_CX_FLAGS "${CMAKE_CXX_FLAGS} /O3")
message(${CMAKE_C_COMPILER})
message(${CMAKE_CXX_COMPILER})
message(${CMAKE_MAKE_PROGRAM})

message("===================================================")

#--------------------------------------------------------------
# include CMake utilities
#--------------------------------------------------------------
include("cmake/collect_source_code_and_resources.cmake")
include("tools/fmt.cmake")
include("tools/glfw.cmake")
include("tools/glm.cmake")
include("tools/spdlog.cmake")
include("tools/rapidjson.cmake")
include("tools/ryml.cmake")
include("tools/pugixml.cmake")
include("tools/pcre2.cmake")

include("tools/tree-sitter.cmake")
include("tools/tree-sitter-cpp.cmake")

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
list(APPEND KERNEL 

"source/application" 
"source/core" 
"source/editor" 
"source/vendor"
)

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(
    OpenGL
    
    # ImGUI
    tools/imgui/imconfig.h
    tools/imgui/imgui.h
    tools/imgui/misc/cpp/imgui_stdlib.h
    tools/imgui/misc/cpp/imgui_stdlib.cpp
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

    "source/main.cpp"
    #"source/binary_to_compressed_c.cpp"
    )
target_link_libraries(
    OpenGL

    # OpenGL backend
    glfw

    # FMT
    fmt::fmt

    # GLM
    glm::glm

    # RYML
    ryml::ryml
    pugixml::pugixml

    # SPDLOG
    spdlog::spdlog

    # PCRE
    pcre2-8
    pcre2-16
    pcre2-32

    # tree-sitter
    tree-sitter
    tree-sitter-cpp
    )

target_include_directories(
    OpenGL 
    PUBLIC ${DIRECTORIES} 
    
    #GLAD
    "source/vendor/glad/include/"
    "tools/glad/include/"

    #IMGUI
    "tools/imgui/"
    "tools/imgui/backends/"
    tools/imgui/misc/cpp/
    
    #RAPIDJSON
    "tools/rapidjson/include"
    )

#--------------------------------------------------------------
# configure CPack
#--------------------------------------------------------------
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
