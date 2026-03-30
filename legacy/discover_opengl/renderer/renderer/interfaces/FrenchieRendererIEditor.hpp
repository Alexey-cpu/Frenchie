#pragma once

namespace Frenchie
{
    namespace Renderer
    {
        class IEditor
        {
        public:
            IEditor(){}
            virtual ~IEditor(){}

            // virtual API
            virtual void draw_editor() = 0;
        };
    }
}