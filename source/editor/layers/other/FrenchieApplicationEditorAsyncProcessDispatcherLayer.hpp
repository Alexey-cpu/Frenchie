#pragma once

#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Async
            {
                class ProcessDispatcher : public Layer
                {
                public:
                    ProcessDispatcher();
                    virtual ~ProcessDispatcher();

                    // Frenchie::Application::Layer
                    virtual void frame_update() override;
                    virtual bool allows_multiple_instances() const;
                };
            }
        }
    }
}