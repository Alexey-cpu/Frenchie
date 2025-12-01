#pragma once

// STL
#include <string>
#include <memory>
#include <list>

namespace Frenchie
{
    namespace Application
    {
        class Layer
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
            virtual void frame_render();
            virtual void frame_finish();
            virtual void finish();
            virtual void quit();
            virtual bool allows_multiple_instances() const;

        protected:

            // info
            std::string m_Name   = "DefaultLayer";
            bool        m_Opened = true;
            bool        m_Shown  = true;
        };
    };    
};