#pragma once

#include <ImmediateUserInterfaceLayer.hpp>

#include <filesystem>

namespace Frenchie
{
    namespace Application
    {
        class ImmediateUserInterfaceTestLayer : public Layer
        {
        public:
            ImmediateUserInterfaceTestLayer();
            virtual ~ImmediateUserInterfaceTestLayer();

            virtual bool awake() override;
            virtual void frame_update() override;
            virtual void finish() override;

            void windows_test();
            void widgets_test();
            void develop_test();

        protected:

            void renderDirectory(const std::filesystem::path& _Path);

            ApplicationRenderingBackendTexture m_FolderClosedTexture;
            ApplicationRenderingBackendTexture m_FolderOpenedTexture;
            ApplicationRenderingBackendTexture m_FileTexture;

            std::shared_ptr<ImmediateUserInterfaceContextLayer> m_ImmediateUserInterface{nullptr};
        };
    }
}