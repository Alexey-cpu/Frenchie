#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

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
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            bool                                           DoubleClicked{false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};
        };

        struct ApplicationMouseCursor
        {
            bool      Entered           {false};
            gs_vec2f  Position          {gs_vec2f(0.f)};
            gs_vec2f  MousePressPosition{gs_vec2f(0.f)};
            gs_vec2f  DragDelta         {gs_vec2f(0.f)};
        };

        struct ApplicationWindow
        {
            bool Focused {false};
        };

        struct ApplicationKey
        {
            enum Key : int
            {
                ApplicationKey_None,
                ApplicationKey_End,
            };

            int                                            Clicks       {0    };
            bool                                           Down         {false};
            bool                                           Hold         {false};
            bool                                           Pressed      {false};
            bool                                           Released     {false};
            bool                                           Clicked      {false};
            std::chrono::high_resolution_clock::time_point PressTime    {std::chrono::high_resolution_clock::time_point()};
            std::chrono::high_resolution_clock::time_point ReleaseTime  {std::chrono::high_resolution_clock::time_point()};
        };

        struct ApplicationInput
        {            
            ApplicationMouseButton MouseButtons[ApplicationMouseButton::Button::ApplicationMouseButton_End]{};
            ApplicationMouseCursor MouseCursor {ApplicationMouseCursor()};
            ApplicationWindow      Window      {ApplicationWindow()};
            ApplicationKey         Keys        [ApplicationKey::Key::ApplicationKey_End]{};
        };

        class ApplicationInstance
        {
        public:
            ApplicationInstance();
            virtual ~ApplicationInstance();

            // API
            bool awake();
            void frame_start();
            void frame_update();
            void frame_debug();
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

            std::list<std::shared_ptr<Layer>> m_Layers;
            std::list<std::shared_ptr<Layer>> m_Awakes;
        };

        Frenchie::Application::ApplicationInstance* application();
    };
};