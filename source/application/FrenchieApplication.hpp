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

        struct ApplicationInput
        {
            bool     WindowFocused  {false};
            
            ApplicationMouseButton MouseButtons[ApplicationMouseButton::Button::ApplicationMouseButton_End]{};
            ApplicationMouseCursor MouseCursor {ApplicationMouseCursor()};
        };

        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            // getters
            // std::string get_name() const;
            gs_vec2f    get_size() const;

            gs_vec2f get_cursor_position() const
            {
                return m_Input.MouseCursor.Position;
            }

            bool is_mouse_button_down(ApplicationMouseButton::Button _Button) const
            {
                return m_Input.MouseButtons[_Button].Down;
            }

            bool is_mouse_button_pressed(ApplicationMouseButton::Button _Button) const
            {
                return m_Input.MouseButtons[_Button].Pressed;
            }

            bool is_mouse_button_released(ApplicationMouseButton::Button _Button) const
            {
                return m_Input.MouseButtons[_Button].Released;
            }

            bool is_mouse_button_clicked(ApplicationMouseButton::Button _Button) const
            {
                return m_Input.MouseButtons[_Button].Clicked;
            }

            bool is_mouse_button_double_clicked(ApplicationMouseButton::Button _Button) const
            {
                return m_Input.MouseButtons[_Button].DoubleClicked;
            }

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

                // awake layer
                if(m_Context != nullptr)
                {
                    if(!layer->awake())
                        return nullptr;
                }

                // push layer into layers stack
                m_Layers.push_back(layer);
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
            void*                             m_Context{nullptr};
        };

        Frenchie::Application::ApplicationInstance* application();
        std::shared_ptr<RenderingQueue>             application_rendering_queue();
    };
};