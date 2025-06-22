#pragma once

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
            Layer(const std::string& _Name);
            virtual ~Layer();

            std::string get_name() const;

            virtual bool awake();
            virtual void frame_start();
            virtual void frame_update();
            virtual void frame_finish();
            virtual void finish();
            virtual void close();
            virtual bool is_closed();

        protected:
            std::string m_Name;
            bool        m_Closed = false;
        };
    };    
};