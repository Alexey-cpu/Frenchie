# Contents

- [Description](#description)
- [Alternatives](#alternatives)

## **Description**

Frenchie is C++ micro framework for development applications with graphical user interface (GUI). The main aim of the project is to provide lightweight, simple and straightforward capabilities for GUI applications development in modern C++.

Frenchie provides layered appication loop that executes range of layer processing functions untill the application is closed. Every application layer is responsible for a limited scope of functions and operations. For example, Frenchie rendering queue and GUI module are implemented as separate layers that interact each other. For context window creation and rendering Frenchie uses platform and graphics backends.

Platform backend abstracts system specific functions for context window creation, manipulation and events catching. Frenchie can use the following open source libraries as platform backend:

| Backend  | Support  |
| -------- | ---------|
| SDL3     | &#x2714; |
| GLFW     | &#x2714; |

Graphics backend abstracts graphics processing unit (GPU) rendering API and is in charge of loading stuff on GPU for rendering. For now, following graphics API's are supported:

| Backend  | Support  |
| -------- | ---------|
| OpenGL   | &#x2714; |
| Vulkan   | &#x2718; |
| DirectX  | &#x2718; |
| Metal    | &#x2718; |

## **Alternatives**

This project has been inspired by Dear ImGUI (https://github.com/ocornut/imgui) and Nuklear (https://github.com/vurtun/nuklear) libraries although there are more alternatives:

| Name     | Purpose                                                                 | link                                         |
| ---------|-------------------------------------------------------------------------| ---------------------------------------------|
| ImGUI    | Tools and data driven GUI applications development                      | https://github.com/ocornut/imgui             |
| Nuklear  | Tools and data driven GUI applications development                      | https://github.com/Immediate-Mode-UI/Nuklear |
|Qt        | Cross-platform application development framework                        | https://www.qt.io/                           |
|WxWidgets | Cross-platform application development framework                        |https://wxwidgets.org/                        |