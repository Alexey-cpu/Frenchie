# Contents

- [Description](#description)

- [Getting started](#getting-started)
    - [Architecture](#architecture)
    - [Setup CMake project](#setup-cmake-project)
    - [Launching application loop](#launching-application-loop)
    - [Coding a simple GUI](#coding-a-simple-gui)
- [Alternatives](#alternatives)

## **Description**

Frenchie is C++ micro framework for development applications with graphical user interface (GUI). The main aim of the project is to provide lightweight, simple and straightforward capabilities for GUI applications development in modern C++.

## **Getting started**

### **Architecture**

Frenchie provides layered appication loop that executes range of layer processing functions untill the application is closed. Every application layer is responsible for a limited scope of functions and operations. For example, Frenchie rendering queue and GUI module are implemented as separate layers that interact each other. Besides layers Frenchie uses platform and rendering backends.

Platform backend abstracts system specific functions for context window creation, manipulation and events catching. Graphics backend abstracts graphics processing unit (GPU) rendering API and is in charge of loading stuff on GPU for rendering. Thus, to start using Frenchie as a driver of GUI in your application you need:

1 Integrate Frenchie into your project as CMake subdirectory

2 Setup platform and rendering backends you want to use

3 Push GUI rendering layer into application loop

4 Launch application loop

### **Setup CMake project**

Before configuring Frenchie micro framework you need to setup common CMake and C/C++ options:

``` CMake
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
```
Frenchie is C++17 project, so to start using it, you need C++17 compatible compiler and CMake 3.3 or newer. As far, as CMake and C++ options are set, you need to configure Frenchie. As it was mentioned in previous section Frenchie uses platform and rendering backends for system specific layers abstraction.

To setup platform backend you need to specify **FRENCHIE_PLATFORM_BACKEND** string variable value. The following table shows which libraries can be used as platform backends and the values you need to setup for **FRENCHIE_PLATFORM_BACKEND** to start using them:

| Backend  |FRENCHIE_PLATFORM_BACKEND |
| -------- |--------------------------|
| SDL3     |SDL                       |
| GLFW     |GLFW                      |

To setup rendering backend you need to specify **FRENCHIE_GRAPHICS_BACKEND** string variable value. The following table shows which graphics API can be used as rendering backends and the values you need to setup for **FRENCHIE_GRAPHICS_BACKEND** to start using them.

| Backend  |FRENCHIE_GRAPHICS_BACKEND |
| -------- |--------------------------|
| OpenGL3  |OPENGL                    |

Besides, you may want to specify how to build Frenchie either as static or shared library. This is specified by **FRENCHIE_BUILD_STATIC_LIBRARY** boolean variable. The following CMake code snippet shows how to configure Frenchie to use SDL library as platform backend and OpenGL graphics API as rendering backend:
``` CMake
#--------------------------------------------------------------
# setup Frenchie library
#--------------------------------------------------------------
set(FRENCHIE_BUILD_STATIC_LIBRARY ON       CACHE BOOL   "Set libary type" FORCE)
set(FRENCHIE_PLATFORM_BACKEND     "SDL"    CACHE STRING "Set platform backend" FORCE)
set(FRENCHIE_GRAPHICS_BACKEND     "OPENGL" CACHE STRING "Set rendering backend" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/../../frenchie/" frenchie_build)
```
After common project options and Frenchie options are set we need to collect project source code files and use them to build and application executable. Frenchie provides handy CMake macro - **collect_source_code_and_resources.cmake** for this purpose. The macro can collect C/C++ source code files from a given lists of directories and/or source code files. The following CMake code snippet shows how to use **collect_source_code_and_resources.cmake** macro to collect source code from a given directory and use the collected files to force CMake to build an application executable:
``` CMake
#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
# this is a really handy macro that collects
# source code within predefined list of directories
include("${CMAKE_SOURCE_DIR}/../../frenchie/cmake/collect_source_code_and_resources.cmake")

list(APPEND KERNEL "source/")

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PRIVATE Frenchie)
```

The whole CMakeLists.txt file that configures Frenchie and forces CMake to build an executable of your project is shown bellow:

``` CMake
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
# setup Frenchie library
#--------------------------------------------------------------
set(FRENCHIE_BUILD_STATIC_LIBRARY ON       CACHE BOOL   "Set libary type" FORCE)
set(FRENCHIE_PLATFORM_BACKEND     "SDL"    CACHE STRING "Set platform backend" FORCE)
set(FRENCHIE_GRAPHICS_BACKEND     "OPENGL" CACHE STRING "Set rendering backend" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/../../frenchie/" frenchie_build)

#--------------------------------------------------------------
# add executable
#--------------------------------------------------------------
# this is a really handy macro that collects
# source code within predefined list of directories
include("${CMAKE_SOURCE_DIR}/../../frenchie/cmake/collect_source_code_and_resources.cmake")

list(APPEND KERNEL "source/")

set(PATHS "")
list(APPEND PATHS ${KERNEL} ${TOOLS})
cmake_language(CALL COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PRIVATE Frenchie)
```

The example project using the above CMakeLists.txt is located in **examples/FrenchieGUIExampleProject** folder. See this folder contents to dive into further details of CMake Frenchie project configuration.

### **Launching application loop**

Frenchie is layered application loop that uses platform and rendering backends for context window creation, manipulation and events catching and for rendering within it. So, to launch context window, it's enough to do what is presented within the following C++ code snippet:

``` C++
#include <FrenchieApplication.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    return Frenchie::Application::application()->execute();
}
```
The code above opens default context window:
# <p style="text-align:center;">![PowercatLogo.](doc/png/frenchie_doc_context_window_example.png)</p>

### **Coding a simple GUI**

To start coding GUI, you can create separate layer that pushes GUI management layer within application loop and provides a client code of your GUI. It's quite a good practice to create a separate layer for each application window. The following code snippet shows how to create layer for window displaying some buttons:
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
            if(m_UI->push_button(m_UI->next_id("Button", "Button")))
                m_Checked = !m_Checked;

            m_UI->check_button(
                m_UI->next_id("Checkbox"),
                m_Checked,
                Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                | Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox);

            m_UI->check_button(
                m_UI->next_id("RadioButton"),
                m_Checked,
                Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                | Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton);

            m_UI->check_button(
                m_UI->next_id("SliderButto"),
                m_Checked,
                Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable
                | Frenchie::Application::ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton);

            m_UI->end_window();
        }
    }

    std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_UI{nullptr};

    bool m_Checked = true;
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

For more information on Frenchie GUI module, system and graphics API see appropriate sections of this doc.

## **API**

### **Core**

Frenchie core source code is located within **/frenchie/source/core** direcotry. The core functionality contains 2D/3D math library for projection matrixes and vectors manipulation, string utilities and miscellaneous components.

#### FrenchieCoreMath

FrenchieCoreMath implementes general simple linear algebra library used for 2D/3D rendering. The library is a compact analog of GLM library (see https://github.com/g-truc/glm).

## **Alternatives**

This project has been inspired by Dear ImGUI (https://github.com/ocornut/imgui) and Nuklear (https://github.com/vurtun/nuklear) libraries although there are more alternatives:

| Name      | Purpose                                                                 | link                                         |
| ----------|-------------------------------------------------------------------------| ---------------------------------------------|
| ImGUI     | Tools and data driven GUI applications development                      | https://github.com/ocornut/imgui             |
| Nuklear   | Tools and data driven GUI applications development                      | https://github.com/Immediate-Mode-UI/Nuklear |
| Qt        | Cross-platform application development framework                        | https://www.qt.io/                           |
| WxWidgets | Cross-platform application development framework                        | https://wxwidgets.org/                       |