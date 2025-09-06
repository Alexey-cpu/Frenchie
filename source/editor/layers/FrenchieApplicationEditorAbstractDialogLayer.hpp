#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Dialog : public Layer
            {
            public:
                Dialog(
                    const std::string& _Name, 
                    const float&       _MinWidth  = 512, 
                    const float&       _MinHeight = 512);
                
                virtual ~Dialog();

                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const;

                virtual void draw_content() = 0;
                virtual void draw_buttons() = 0;

            protected:
                // info
                float m_MinWidth  = 512;
                float m_MinHeight = 512;
            };
        }
    }
}