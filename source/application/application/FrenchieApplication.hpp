#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        class Application
        {
        public:
            Application();
            virtual ~Application();

            // getters
            std::string get_name() const;

            // setters
            void set_name(const std::string& _Name);

            virtual bool awake()        = 0;
            virtual void frame_start()  = 0;
            virtual void frame_update() = 0;
            virtual void frame_finish() = 0;
            virtual void finish()       = 0;
            
            virtual bool is_closed();
            virtual void close();
            
            // native API
            int execute();

            template<typename __type, typename ... __parameters>
            std::shared_ptr<__type> push(__parameters... _Parameters)
            {
                std::shared_ptr<__type> layer = 
                    std::make_shared<__type>(_Parameters ...);
                layer->awake();
                m_Layers.push_back(layer);
                return layer;
            }

            template<typename __type>
            bool contains()
            {
                return std::find_if(
                        m_Layers.begin(),
                        m_Layers.end(),
                        [](std::shared_ptr<Layer> _Layer)->bool
                        {
                            return std::dynamic_pointer_cast<__type>(_Layer) != nullptr;
                        }) != m_Layers.end();
            }

        protected:
            std::list<std::shared_ptr<Layer>> m_Layers =  std::list<std::shared_ptr<Layer>>();
            std::string                       m_Name   = "Frenchie::Application";
            bool                              m_Closed = false;
        };
    };    
};