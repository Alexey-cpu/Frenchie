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

            // this is layer initialization function
            // that is called once when the layer is 
            // pushed into application pipeline
            virtual bool awake();

            // this function is needed to prepare layer
            // for current frame processing:
            // compute geometry, compute projection matrixes, detect events e.t.c
            virtual void frame_start();

            // this function is needed to update the layer
            // within current frame: modify geometry,
            // compute transform matrixes, prepare rendering commands queue e.t.c
            virtual void frame_update();

            // TODO: this is not a debug function !!!
            virtual void frame_debug();

            // this function is needed to execute rendering commands
            virtual void frame_render();

            // this function is needed basically for application events processing
            virtual void frame_finish();

            // this function is needed for safe and reliable layer processing stop
            virtual void finish();

            // this function is needed for application layer resources clean-up
            virtual void quit();

            // 
            virtual bool allows_multiple_instances() const;

        protected:

            // info
            std::string m_Name   = "DefaultLayer";
            bool        m_Opened = true;
            bool        m_Shown  = true;
        };
    };    
};