#pragma once

#include <string>
#include <memory>
#include <list>

// Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreStringConvert.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

namespace Frenchie
{
    namespace Application
    {
        class Layer : public Frenchie::Core::Factory::Creator<Layer>
        {
        public:
            Layer(const std::string& _Name = std::string());
            virtual ~Layer();

            // getters
            std::string get_name() const;

            // setters
            void set_name(const std::string&);

            // API
            bool is_closed() const;
            bool is_hidden() const;
            void close();
            void hide();
            void show();

            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();
            virtual void finish();
            virtual bool allows_multiple_instances() const;

        protected:

            // info
            std::string m_Name   = STRINGIFY(Layer);
            bool        m_Opened = true;
            bool        m_Shown  = true;
        };
    };    
};