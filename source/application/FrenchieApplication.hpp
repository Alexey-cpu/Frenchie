#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationLayerRenderingQueue.hpp>

// STL
#include <iostream>
#include <chrono>

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationMouseButton
        {
            enum Button : int
            {
                ApplicationMouseButton_Begin,
                ApplicationMouseButton_Left = ApplicationMouseButton_Begin,
                ApplicationMouseButton_Right,
                ApplicationMouseButton_Middle,
                ApplicationMouseButton_End
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            bool                                           DoubleClicked{false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};

            
        };

        struct ApplicationMouseCursor
        {
            enum Cursor : int
            {
                ApplicationMouseCursor_Begin,
                ApplicationMouseCursor_Arrow = ApplicationMouseCursor_Begin,
                ApplicationMouseCursor_TextInput,
                ApplicationMouseCursor_Crosshair,
                ApplicationMouseCursor_PointingHand,
                ApplicationMouseCursor_HorizontalDoubleHeaded,
                ApplicationMouseCursor_VerticalDoubleHeaded,
                ApplicationMouseCursor_TopLeftToBottomRightHeaded,
                ApplicationMouseCursor_TopRighToBottomLeftHeaded,
                ApplicationMouseCursor_End
            }         View    {ApplicationMouseCursor_Arrow};
            bool      Entered {false};
            gs_vec2f  Position{gs_vec2f(0.f)};
            uintptr_t Cursors[ApplicationMouseCursor_End]{};
        };

        struct ApplicationWindow
        {
            bool Focused {false};
        };

        struct ApplicationInput
        {            
            ApplicationMouseButton MouseButtons[ApplicationMouseButton::Button::ApplicationMouseButton_End]{};
            ApplicationMouseCursor MouseCursor {ApplicationMouseCursor()};
            ApplicationWindow      Window      {ApplicationWindow()};
        };

        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            // getters
            std::string get_window_name() const;
            gs_vec2f    get_window_size() const;
            gs_vec2f    get_window_position() const;
            gs_vec2f    get_window_cursor_position() const;

            bool is_mouse_button_down(const ApplicationMouseButton::Button&) const;
            bool is_mouse_button_pressed(const ApplicationMouseButton::Button&) const;
            bool is_mouse_button_released(const ApplicationMouseButton::Button&) const;
            bool is_mouse_button_clicked(const ApplicationMouseButton::Button&) const;
            bool is_mouse_button_double_clicked(const ApplicationMouseButton::Button&) const;

            // setters
            void set_window_name(const std::string&);

            // API
            bool awake();
            void frame_start();
            void frame_update();
            void frame_render();
            void frame_finish();
            void finish();
            void quit();
            bool is_closed();
            void close();

            int execute();
            std::list<std::shared_ptr<Layer>>::const_iterator begin() const;
            std::list<std::shared_ptr<Layer>>::const_iterator end() const;
            size_t size() const;

            // layers
            template<typename Type, typename ... Arguments>
            std::shared_ptr<Type> push_layer(Arguments... _Parameters)
            {
                // create layer
                auto layer = std::make_shared<Type>(_Parameters...);

                // check if layer allows multiple instances
                if(contains_layer<Type>() && !layer->allows_multiple_instances())
                    return find_layer<Type>();

                m_Awakes.push_back(layer);
                return layer;
            }

            template<typename Type>
            std::shared_ptr<Type> find_layer()
            {
                auto layer = std::find_if(
                    m_Layers.begin(),
                    m_Layers.end(),
                    [](std::shared_ptr<Layer> _Layer)->bool
                    {
                        return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                    }
                );

                return layer != m_Layers.end() ? std::dynamic_pointer_cast<Type>(*layer) : nullptr;
            }

            template<typename Type>
            bool contains_layer()
            {
                return std::find_if(
                        m_Layers.begin(),
                        m_Layers.end(),
                        [](std::shared_ptr<Layer> _Layer)->bool
                        {
                            return std::dynamic_pointer_cast<Type>(_Layer) != nullptr;
                        }) != m_Layers.end();
            }

            typedef std::list<std::shared_ptr<Layer>>::const_iterator const_iterator;

        protected:

            friend class ApplicationInputHandler;

            ApplicationInput                  m_Input  {ApplicationInput()};
            std::list<std::shared_ptr<Layer>> m_Layers {std::list<std::shared_ptr<Layer>>()};
            std::list<std::shared_ptr<Layer>> m_Awakes {std::list<std::shared_ptr<Layer>>()};
            void*                             m_Context{nullptr};
        };

        Frenchie::Application::ApplicationInstance* application();
        std::shared_ptr<RenderingQueue>             application_rendering_queue();
    };
};