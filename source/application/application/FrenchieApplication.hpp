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
            Application(){}
            virtual ~Application(){}

            virtual bool awake()        = 0;
            virtual void frame_start()  = 0;
            virtual void frame_update() = 0;
            virtual void frame_finish() = 0;
            virtual void finish()       = 0;
            virtual bool is_closed()    = 0;
            virtual void close()        = 0;
            
            // native API
            int execute()
            {
                if(!awake()) 
                    return -1;

                while (!is_closed())
                {
                    // remove layers that are closed
                    for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
                    {
                        if((*it)->is_closed())
                        {
                            (*it)->finish();
                            auto rm = it;
                            it++;
                            m_Layers.erase(rm);

                            if(it == m_Layers.end())
                                break;
                        }
                    }

                    frame_start();
                    frame_update();
                    frame_finish();
                }

                finish();

                return 1;
            }

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
            std::list<std::shared_ptr<Layer>> m_Layers = 
                std::list<std::shared_ptr<Layer>>();
        };
    };    
};