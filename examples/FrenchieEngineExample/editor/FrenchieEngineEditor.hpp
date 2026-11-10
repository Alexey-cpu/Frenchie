#pragma once

#pragma once

// Application
#include <FrenchieImmediateUserInterfaceLayer.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplication.hpp>

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreStringUtilities.hpp>

namespace Frenchie
{
    namespace Engine
    {
        using namespace Frenchie::Application;

        class Editor final : public Layer
        {
        public:
            Editor();
            virtual ~Editor();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_input() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;

            template<typename Type>
            static void render_editor_of(const Type*);

        private:
            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_UI{nullptr};
        };
    }
}