#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class ProcessesDispatcher : public Frenchie::Application::Layer
        {
        public:
            ProcessesDispatcher();
            virtual ~ProcessesDispatcher();

            // Frenchie::Application::Layer
            virtual void frame_update() override;
            virtual bool allows_multiple_instances() const;
        };
    }
}