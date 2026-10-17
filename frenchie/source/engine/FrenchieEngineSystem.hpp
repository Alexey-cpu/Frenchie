#pragma once

// Engine
#include <FrenchieEngineScene.hpp>

namespace Frenchie
{
    namespace Engine
    {
        using namespace Frenchie::Engine;

        class System
        {
        public:

            System(){}
            virtual ~System(){}

            virtual bool awake(Scene&){return true;}
            virtual void frame_start(Scene&){}
            virtual void frame_update(Scene&){}
            virtual void frame_input(Scene&){}
            virtual void frame_render(Scene&){}
            virtual void frame_finish(Scene&){}
            virtual void finish(Scene&){}
            virtual void quit(Scene&){}
        };
    }
}