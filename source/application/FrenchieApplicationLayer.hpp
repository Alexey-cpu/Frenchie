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

            // This is layer initialization function that is called once when the layer is pushed into application pipeline
            virtual bool awake();

            // In this function we prepare layer for current frame processing. Ususally we compute projection and camera matrixes here
            virtual void frame_start();

            // In this function we usually modify geometry, compute transform matrixes, prepare rendering commands e.t.c
            virtual void frame_update();

            // In this function we usually catch application input and events
            virtual void frame_input();

            // In this function rendering commands are executed. Essentially in sends all geometry and shader paramters onto GPU
            virtual void frame_render();

            // In this function we usually do post processing after rendering
            virtual void frame_finish();

            // In this function we safelly stop layer before it's poped out of application pipeline
            virtual void finish();

            // In this functrion we safelly clean-up layer resources when application is closedf
            virtual void quit();

            // This function indentified if the layer can have multiple instances
            virtual bool allows_multiple_instances() const;

        protected:

            // info
            std::string m_Name   = "DefaultLayer";
            bool        m_Opened = true;
            bool        m_Shown  = true;
        };
    };    
};