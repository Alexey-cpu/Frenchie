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
                Dialog(const std::string&);
                virtual ~Dialog();

                virtual void frame_update() override;
                virtual void draw_content() = 0;
                virtual void draw_buttons() = 0;
            };
        }
    }
}