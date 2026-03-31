# Contents

- [Overview](#overview)
    - [Description](#description)
    - [Supported platform backends](#Supportedplatformbackends)
    - [Supported graphics backends](#Supportedgraphicsbackends)

## **Description**

Frenchie is C++ micro framework for development applications with graphical user interface (GUI). The project has been inspired by Dear ImGUI (https://github.com/ocornut/imgui) and Nuklear (https://github.com/vurtun/nuklear) libraries. Although these libraries propose simple and straightforward way to build GUI applications they are quite challenging to start with for those who does not have much background in computer graphics and GUI applications development. The main aim of this projects is to provide simple and robust way for building GUI applications.

Frenchie is organized as layered appication loop that executes a bunch of layer processing functions untill the application is closed. Every application layer is responsible for a limited scrope of functions and operations. Besides layers, application uses platform and graphics backends.

Platform backend abstracts system specific functions used for context window creation, manipulation and it's events catching. Graphics backend abstracts graphics processing unit (GPU) rendering API and is in charge of loading stuff on GPU for rendering.

## **Supported platform backends**

Frenchie platform backend implementation can use the following external libraries as platform backend:

| Backend  | Support  |
| -------- | ---------|
| SDL3     | &#x2714; |
| GLFW     | &#x2714; |

## **Supported graphics backends**

In current version of Frenchie micro framework only OpenGL graphics API is supported. All other API support is in active development:

| Backend  | Support  |
| -------- | ---------|
| OpenGL   | &#x2714; |
| Vulkan   | &#x2718; |
| DirectX  | &#x2718; |
| Metal    | &#x2718; |