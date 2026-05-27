[English](README.md) | [Russian](README.ru.md)

[![License](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](http://opensource.org/licenses/MIT)
[![API reference](https://img.shields.io/badge/view-API_reference-blue)](https://alexey-cpu.github.io/Frenchie/)

Frenchie
=====

<p align="center">Компактный C++ framewok для разработки приложений с графическим интерфейсом пользователя.</p>

---

<p align="center"><img src="doc/png/frenchie_logo.png" align="center" width="50%"></p>


- [Общие сведения](#общие-сведения)

- [Начало работы](#getting-started)
    - [Требования к набору инструментов](#требования-к-набору-инструментов)
    - [Как это работает](#как-это-работает)
    - [Пример создания простого проекта](#пример-создания-простого-проекта)
- [Другие похожие проекты](#другие-похожие-проекты)

## **Общие сведения**

Frenchie - **это компактный C++ framework для разработки приложений с графическим интерфейсом пользователя**. Основная цель проекта - создать простой в использовании набор инструментов, позволяющий быстро создавать приложения с графическим интерфейсом пользователя на языке C++ для множества платформ.

## **Начало работы**

### **Требования к набору инструментов**

Frenchie является библиотекой на C++, которая использует систему сборки CMake. Для того, чтобы начать работать с Frenchie, нужно установить компилятор с поддержкой C++ 17 и выше, а также CMake 3.3 или новее.

### **Как это работает**

Frenchie является бесконечным циклом, который обрабатывает компоненты приложения (слои) до тех пор, пока приложение не закроется. Каждый слой приложения реализует ограниченный набор функций. Для запуска контекстного окна и рисования графики в нем, Frenchie использует системный и графический backend-ы. Системный backend представляет из себя слой абстракции над платформо-зависимым функционалом операционной системы, при помощи которого открывается контекстное окно и производится отслеживание его событий. Графический backend - это слой абстракции над графическим API, типа OpenGL, DirectX, Vulkan, Metal. Графический backend отвечает за загрузку геометрии и текстур на видеокарту для последующей рисовки.

### **Пример создания простого проекта**

При создании CMake проекта рекомендуется использовать модуль **CMake FetchContent** для скачивания и конфигурирования актуальной версии Frenchie. Структура папок проекта должна быть такой:

```
Project/
|-- source
|--|--main.cpp
|--tools
|--|--frenchie.cmake
└── CMakeLists.txt
```
В папке **source/** должны располагатся файлы с исходным кодом проекта, а в папке **tools/** - **.cmake** файлы, предназначенные для скачивания и конфигурирования зависимостей проекта с использованием модуля **CMake FetchContent**. В данном простом проекте всего одна зависимость - Frenchie и файл **frenchie.cmake** будет выглядеть так:

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

Вышеуказанный код на языке CMake конфигурирует Frenchie так, чтобы он собирался как статическая библиотека и использовал **SDL3** в качестве системного backend-а и **OPENGL3** в качестве графического backend-а.

На данный момент, Frenchie использует сторонние библиотеки в качестве системного backend-а. Ниже представлен список сторонниз библиотек, которые можно использовать в качестве системного backend-а:

| Backend  |FRENCHIE_PLATFORM_BACKEND |
| -------- |--------------------------|
| SDL3     |SDL3                      |
| GLFW     |GLFW                      |

Поддерживаемые графические API представлены в таблице ниже:

| Backend     |FRENCHIE_GRAPHICS_BACKEND |
| ------------|--------------------------|
| OpenGL3     |OPENGL3                   |
| DirectX9    |DIRECTX9                  |
| MacOS Metal |METAL                     |

Для создания простого приложения достаточно включить файл **frenchie.cmake** в **CMakeLists.txt** файл C++ проекта, добавить **Frenchie** как поддиректорию, и далее собрать список файлов с исходным кодом проекта и передать его в функцию СMake, которая создает проект исполняемого файла приложения. ниже представлен код, который выполняет вышеуказанные процедуры:

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
Вышеприведенный код конфигурирует CMake на создание исполняемого файла приложения с использованием исходного кода, расположенного в папке **source/**. Для создания списка файлов с исходным кодом и подключаемый к проекту диреторий, Frenchie предоставляет удобный cmake макрос, расположенный в файле **collect_source_code_and_resources.cmake**. Данный макрос умеет собирать в список одиночные C/C++ и Objective-C/C++ файлы, а также искать эти файлы в заданных директориях.

Далее, чтобы начать разрабатывать приложение, требуется создать слой, который будет добавлять слой GUI в цикл приложения. Рекомендуется для каждого отдельного окна приложения создавать отдельный слой. Ниже представлен код, который сосздает простое окно с редактором цвета:

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
                m_UI->next_height(m_UI->get_text_line_height());

                if(m_UI->begin_horizontal_stack(m_UI->next_id("Combobox")))
                {
                    auto parentBox = m_UI->current_bounding_box(m_UI->get_rendering_stack_top()).size();

                    m_UI->label(m_UI->next_id("ColorPickerType"), "Type");

                    m_UI->next_size(512.f);

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

                if(m_RGBAColorPicker)
                    m_UI->color_picker_rgba(m_UI->next_id("RGBAColorPicker"), m_ColorPickerColor);
                else
                    m_UI->color_picker_hsva( m_UI->next_id("HSVAColorPicker"), m_ColorPickerColor);

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
В результате, должно получится такое окно:

![Watch the video](doc/gif/frenchie_doc_simple_window_example.gif)

Больше примеров работы с **Frenchie** можно найти в папке **examples/**. Детальное описание API Frenchie находится по ссылке **API reference**.

## **Другие похожие проекты**

 Данный проект задумывался как вариант реализации парадигмы immediate mode GUI, которая уже реализована в таких бибилотеках как Dear ImGUI (https://github.com/ocornut/imgui) и Nuklear (https://github.com/vurtun/nuklear). При этом, имеются и другие библиотеки и framework-и для разработки приложений с графическим интерфейсом пользователя, вот некоторые из них

| Название  | Назначение                                                             | Ссылка                                       |
| ----------|------------------------------------------------------------------------| ---------------------------------------------|
| ImGUI     | Разработка интерактивных инструментов и real-time приложений           | https://github.com/ocornut/imgui             |
| Nuklear   | Разработка интерактивных инструментов и real-time приложений           | https://github.com/Immediate-Mode-UI/Nuklear |
| Qt        | Разработка кросс-платформеннных приложений с графическим интерфейсом   | https://www.qt.io/                           |
| WxWidgets | Разработка кросс-платформеннных приложений с графическим интерфейсом   | https://wxwidgets.org/                       |
