#pragma once

// Application
#include <FrenchieImmediateUserInterfaceLayer.hpp>

// Core
#include <FrenchieCoreSerizliation.hpp>

// STL
#include <filesystem>

namespace Frenchie
{
    namespace Application
    {
        class ImmediateUserInterfaceModelLayer : public Layer
        {
        public:
            ImmediateUserInterfaceModelLayer(const std::filesystem::path& _Path);
            virtual ~ImmediateUserInterfaceModelLayer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;

        private:
        
            std::map<std::string, std::any>                                            m_Model;
            Frenchie::Core::Serizliation::DOMTree                                      m_View;
            std::filesystem::path                                                      m_ViewPath;
            Frenchie::Core::Serizliation::DOMTree::Status                              m_ViewStatus;
            std::filesystem::file_time_type                                            m_ViewLastWriteTime;
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_Controller;

            // service methods
            void next_node(const Frenchie::Core::Serizliation::ElementObj& _Object);
            int  layout_hints(const Frenchie::Core::Serizliation::ElementObj& _Object);
            
            bool begin_panel(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_panel(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool begin_vertical_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_vertical_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool begin_horizontal_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_horizontal_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool push_button(const Frenchie::Core::Serizliation::ElementObj& _Object);
        };
    }
}