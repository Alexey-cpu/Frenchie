#pragma once

// Core
#include <FrenchieCoreSingleton.hpp>

// Application
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
            enum Type : int
            {
                ApplicationMouseButton_Begin,
                ApplicationMouseButton_Left = ApplicationMouseButton_Begin,
                ApplicationMouseButton_Right,
                ApplicationMouseButton_Middle,
                ApplicationMouseButton_End
            };

            int                                            Clicks       {0    };
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            bool                                           DoubleClicked{false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};
        };

        struct ApplicationInput
        {
            // ApplicationInput()
            // {
            //     for (int mouseButton = ApplicationMouseButton::ApplicationMouseButton_Begin;
            //              mouseButton < ApplicationMouseButton::ApplicationMouseButton_End;
            //              mouseButton++)
            //     {
            //         MouseButtonPressed      [mouseButton] = false;
            //         MouseButtonReleased     [mouseButton] = false;
            //         MouseButtonClicked      [mouseButton] = false;
            //         MouseButtonDoubleClicked[mouseButton] = false;
            //     }
            // }

            // std::chrono::high_resolution_clock::time_point MousePressTimeStamp;
            // std::chrono::high_resolution_clock::time_point MouseReleaseTimeStamp;
            // std::chrono::high_resolution_clock::time_point MouseClickElapsedTime;
            // double                                         MouseClickDetectThreshold = 300;
            //double CurrentTime = 0;

            gs_vec2f CursorPosition          {gs_vec2f(0.f)};
            gs_vec2f CursorDragDelta         {gs_vec2f(0.f)};
            bool     WindowFocused           {false};
            bool     CursorEntered           {false};
            
            ApplicationMouseButton MouseButtons[ApplicationMouseButton::Type::ApplicationMouseButton_End]{};


            // bool     MouseButtonReleased     [ApplicationMouseButtonType::ApplicationMouseButton_End]{};
            // bool     MouseButtonClicked      [Type::ApplicationMouseButton_End]{};
            // bool     MouseButtonDoubleClicked[Type::ApplicationMouseButton_End]{};
        };

        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            // getters
            std::string get_name() const;
            gs_vec2f    get_size() const;

            // setters
            void set_name(const std::string&);

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
                    {
                        Frenchie::Core::Logger::instance()->error(fmt::format("Could not awake layer {}", layer->get_name()));
                        return nullptr;
                    }
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

            std::list<std::shared_ptr<Layer>> m_Layers {std::list<std::shared_ptr<Layer>>()};
            void*                             m_Context{nullptr};
            ApplicationInput                  m_Inputs {ApplicationInput()};
        };

        Frenchie::Application::ApplicationInstance* application();
        std::shared_ptr<RenderingQueue>             application_rendering_queue();
    };
};