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

            virtual bool awake()
            {
                for (auto& layer : m_Layers) 
                    layer->awake();

                return true;
            }
            
            virtual void frame_start()
            {
                for (auto& layer : m_Layers) 
                    layer->frame_start();
            }
            
            virtual void frame_update()
            {
                for (auto& layer : m_Layers) 
                    layer->frame_update();
            }
            
            virtual void frame_finish()
            {
                for (auto& layer : m_Layers) 
                    layer->frame_finish();
            }
            
            virtual void finish()
            {
                for (auto& layer : m_Layers) 
                    layer->finish();
            }
            
            virtual void close()
            {
                m_Closed = true;
            }
            
            virtual bool is_closed()
            {
                return m_Closed;
            }

        protected:

            std::list<std::shared_ptr<Layer>> m_Layers = 
                std::list<std::shared_ptr<Layer>>();

            bool m_Closed = false;
        };
    };    
};