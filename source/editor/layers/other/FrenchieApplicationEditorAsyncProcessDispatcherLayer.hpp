#include <FrenchieApplication.hpp>

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
                };
            }
        }
    }
}