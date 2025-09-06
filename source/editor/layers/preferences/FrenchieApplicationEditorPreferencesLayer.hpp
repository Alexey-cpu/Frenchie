#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Core
#include <FrenchieCoreISerializer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace Preferences
            {
                // explorer
                class Explorer : public Layer
                {
                public:
                    Explorer();
                    virtual ~Explorer();

                    // Frenchie::Application::Layer
                    virtual bool awake() override;
                    virtual void frame_update() override;
                    virtual bool allows_multiple_instances() const;

                protected:

                    std::vector<std::shared_ptr<Layer>> m_Topics = 
                        std::vector<std::shared_ptr<Layer>>();
                };
            }
        }
    }
}