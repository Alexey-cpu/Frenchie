[![License](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](http://opensource.org/licenses/MIT)
[![API reference](https://img.shields.io/badge/view-API_reference-blue)](https://alexey-cpu.github.io/Frenchie/)

# Contents

- [Description](#description)

- [Getting started](#getting-started)
    - [Requirements](#requirements)
    - [How it works](#how-it-works)
    - [Creating a simple project](#creating-a-simple-project)
- [Alternatives](#alternatives)

## **Description**

Frenchie is **C++ micro framework for development applications with graphical user interface (GUI)**. The main aim of the project is to provide lightweight, simple and straightforward capabilities for **cross-platform GUI applications development in modern C++**.

## **Getting started**

### **Requirements**

Frenchie is C++/CMake library. To start using it you need C++17 or higher compatible compiler and CMake 3.3 or newer.

### **How it works**

Frenchie is layered appication loop that executes range of layer processing functions untill the application is closed. Every application layer is responsible for a limited scope of functions. To drive context window and graphics Frenchie uses platform and rendering backends. Platform backend abstracts system specific functions for context window creation, manipulation and events catching. Graphics backend abstracts graphics processing unit (GPU) rendering API and is in charge of loading stuff on GPU for rendering.

### **Creating a simple project**

To create a project using Frenchie it's better to use **CMake FetchContent** module to load and configure the latest version of **Frenchie**. A folder structure of a simple **Frenchie** project looks like this:

```
Project/
|-- source
|--|--main.cpp
|--tools
|--|--frenchie.cmake
└── CMakeLists.txt
```
**source/** folder contains the project source code and **tools/** folder contains **.cmake** files that use **CMake FetchContent** for the project dependencies upload and configuring. In this simple project the only dependency is **Frenchie**. So, **frenchie.cmake** file that uploads the latest version of **Frenchie** and configures it should look like this:

``` CMake
include(FetchContent)

# load Frenchie from remote
FetchContent_Declare(
    Frenchie
    GIT_REPOSITORY "https://github.com/Alexey-cpu/Frenchie.git"
    GIT_TAG "origin/v1/release"
    OVERRIDE_FIND_PACKAGE
	GIT_SHALLOW TRUE)

# configure Frenchie
set(FRENCHIE_BUILD_STATIC_LIBRARY ON        CACHE BOOL   "Set libary type" FORCE)
set(FRENCHIE_PLATFORM_BACKEND     "SDL3"    CACHE STRING "Set platform backend" FORCE)
set(FRENCHIE_GRAPHICS_BACKEND     "OPENGL3" CACHE STRING "Set rendering backend" FORCE)

FetchContent_MakeAvailable(Frenchie)
```

The code above configures **Frenchie** to be a static library using **SDL3** as platform backend and **OPENGL3** as graphics backend. Supported platform backends are listed in table bellow:

| Backend  |FRENCHIE_PLATFORM_BACKEND |
| -------- |--------------------------|
| SDL3     |SDL3                      |
| GLFW     |GLFW                      |

Supported graphics backends are the following:

| Backend  |FRENCHIE_GRAPHICS_BACKEND |
| -------- |--------------------------|
| OpenGL3  |OPENGL3                   |

To create a simple executable project it's enough to include **frenchie.cmake** file into appropriate **CMakeLists.txt** file, add **Frenchie** library as a subdirectory, collect source all code and then add executable target linking **Frenchie** library to it. The following **CMakeLists.txt** code snippet shows how to do it:

``` CMake
#--------------------------------------------------------------
# setup CMake
#--------------------------------------------------------------
cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(FrenchieGUIGettingStartedProject VERSION 1.0.0 LANGUAGES C CXX)

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
# setup Frenchie library
#--------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/tools/frenchie.cmake")
add_subdirectory("${frenchie_SOURCE_DIR}/frenchie/")

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
# this is a really handy macro that collects source code within predefined list of directories
include("${frenchie_SOURCE_DIR}/frenchie/cmake/collect_source_code_and_resources.cmake")

list(APPEND KERNEL "source/")

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PUBLIC Frenchie)
```

The code above configures a simple C++17 project that builds executable target out of source code located within **source/** directory. The source code is collected using a handy CMake macro - **collect_source_code_and_resources.cmake**. The macro can collect C/C++ source code files from a given lists of directories and/or source code files.

To start coding GUI it's enough create separate layer that pushes GUI management layer within application loop and provides a client code of your GUI. It's recommended to create a separate layer for each application window. The following code snippet shows how to create layer for window displaying a simple color pciker:

``` C++
#include <FrenchieApplication.hpp>
#include <FrenchieImmediateUserInterfaceLayer.hpp>

class SomeSimpleGuiLayer : public Frenchie::Application::Layer
{
public:
    SomeSimpleGuiLayer() : Frenchie::Application::Layer("TestLayer"){}
    virtual ~SomeSimpleGuiLayer(){}

    virtual bool awake() override
    {
        if(m_UI == nullptr)
            m_UI = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

        return m_UI != nullptr;
    }

    virtual void frame_update() override
    {
        if(m_UI->begin_window(m_UI->next_id("SomeSimpleWindow")))
        {
            m_UI->next_content_margin(gs_vec4f(
                m_UI->m_Style.get_frames_width() + m_UI->m_Style.get_frames_radius() * 0.5f, // top
                m_UI->m_Style.get_frames_width() + m_UI->m_Style.get_frames_radius() * 0.5f, // left
                0.f,  // right
                0.f   // bottom 
            ));

            m_UI->next_content_padding(gs_vec4f(
                m_UI->m_Style.get_frames_width() + m_UI->m_Style.get_frames_radius() * 0.5f, // top
                m_UI->m_Style.get_frames_width() + m_UI->m_Style.get_frames_radius() * 0.5f, // left
                0.f,  // right
                0.f   // bottom 
            ));

            if(m_UI->begin_vertical_stack(
                m_UI->next_id("ColorEditor"),
                Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter
                | Frenchie::Application::ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter))
            {
                m_UI->next_maximum_size(gs_vec2f(gs_huge<float>(), m_UI->m_Style.get_font_size()));

                if(m_UI->begin_horizontal_stack(m_UI->next_id("Combobox")))
                {
                    m_UI->label(m_UI->next_id("ColorPickerType"), "Type");

                    if(m_UI->begin_combobox(m_UI->next_id("Combobox"),m_RGBAColorPicker ? "RGBA" : "HSVA"))
                    {
                        bool rgbaSelected     = m_RGBAColorPicker;
                        bool hsvaSelected     = !m_RGBAColorPicker;
                        int  checkboxSettings = Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

                        m_UI->check_button(m_UI->next_id("RGBASelected"), rgbaSelected, checkboxSettings);
                        m_UI->same_line();
                        if(m_UI->combobox_item(m_UI->next_id("RGBA", "RGBA"))) m_RGBAColorPicker = true;

                        m_UI->check_button(m_UI->next_id("HSVASelected"), hsvaSelected, checkboxSettings);
                        m_UI->same_line();
                        if(m_UI->combobox_item(m_UI->next_id("HSVA", "HSVA"))) m_RGBAColorPicker = false;

                        m_UI->end_combobox();
                    }

                    m_UI->end_horizontal_stack();
                }

                if(m_UI->begin_horizontal_stack(m_UI->next_id("Pickers")))
                {
                    if(m_RGBAColorPicker)
                        m_UI->color_picker_rgba(m_UI->next_id("RGBAColorPicker"), m_ColorPickerColor);
                    else
                        m_UI->color_picker_hsva( m_UI->next_id("HSVAColorPicker"), m_ColorPickerColor);

                    m_UI->end_horizontal_stack();
                }

                m_UI->end_vertical_stack();
            }

            m_UI->end_window();
        }
    }

    std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

    gs_color m_ColorPickerColor = gs_color_rgba(255, 0, 0, 255); // white
    bool     m_RGBAColorPicker  = true;                          // use RGBA color picker

};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Frenchie::Application::application()->push_layer<SomeSimpleGuiLayer>();
    return Frenchie::Application::application()->execute();
}
```

The code above creates the following simple window displaying some buttons:

![Watch the video](doc/gif/frenchie_doc_simple_window_example.gif)

For more examples refer to **examples/** folder of this repo. All examples there are organized using the scheme described in this section.

## **Alternatives**

This project has been inspired by Dear ImGUI (https://github.com/ocornut/imgui) and Nuklear (https://github.com/vurtun/nuklear) libraries, so Frenchie GUI module is implemented in immediate mode paradigm. Anyway, there are other alternatives providing more functionality:

| Name      | Purpose                                                                 | link                                         |
| ----------|-------------------------------------------------------------------------| ---------------------------------------------|
| ImGUI     | Tools and data driven GUI applications development                      | https://github.com/ocornut/imgui             |
| Nuklear   | Tools and data driven GUI applications development                      | https://github.com/Immediate-Mode-UI/Nuklear |
| Qt        | Cross-platform application development framework                        | https://www.qt.io/                           |
| WxWidgets | Cross-platform application development framework                        | https://wxwidgets.org/                       |
