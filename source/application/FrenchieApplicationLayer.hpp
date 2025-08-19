#pragma once

#include <string>
#include <memory>
#include <list>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

namespace Frenchie
{
    namespace Application
    {
        class Layer
        {
        public:
            Layer(const std::string& _Name);
            virtual ~Layer();

            std::string get_name() const;

            // API
            bool is_closed();
            void close();

            // virtual API
            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();
            virtual void finish();
            virtual bool allows_multiple_instances() const;

        protected:
            std::string m_Name;
            bool        m_Opened = true;
        };

        class IMouseCallbackHandler
        {
        public:
            IMouseCallbackHandler(){}
            virtual ~IMouseCallbackHandler(){}
            virtual void mouse_callback(GLFWwindow* _Window, double _X, double _Y) = 0;
        };
    };    
};