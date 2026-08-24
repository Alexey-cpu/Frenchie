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
        class ImmediateUserInterfaceController
        {
        public:
            ImmediateUserInterfaceController(){}
            virtual ~ImmediateUserInterfaceController(){}
            virtual void set_model(std::map<std::string, std::any>&) = 0;
        };

        class ImmediateUserInterfaceModelLayer : public Layer
        {
        public:
            ImmediateUserInterfaceModelLayer(
                const std::filesystem::path&                             _View,
                const std::shared_ptr<ImmediateUserInterfaceController>& _Controller = nullptr);
            virtual ~ImmediateUserInterfaceModelLayer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;

        private:
        
            std::map<std::string, std::any>                                            m_Model;
            Frenchie::Core::Serizliation::Document                                     m_View;
            std::filesystem::path                                                      m_ViewPath;
            Frenchie::Core::Serizliation::Document::Status                             m_ViewStatus;
            std::filesystem::file_time_type                                            m_ViewLastWriteTime;
            std::shared_ptr<ImmediateUserInterfaceController>                          m_Controller;
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_Context;

            // service methods
            void next_node(const Frenchie::Core::Serizliation::ElementObj& _Object);
            int  layout_hints(const Frenchie::Core::Serizliation::ElementObj& _Object);

            template<typename Type, typename Default>
            Type parse_value(const Frenchie::Core::Serizliation::ElementObj& _Object, const Default& _Default)
            {
                Type value = _Default(std::string(_Object.get_value()));

                if(_Object.empty())
                    return value;

                ElementObj sourceObj = _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Source";
                });

                if(!sourceObj.get_value().empty() && m_Model.find(std::string(sourceObj.get_value())) !=  m_Model.end())
                {
                    try
                    {
                        return std::any_cast<Type>(
                            m_Model[std::string(sourceObj.get_value())]);
                    }
                    catch(...)
                    {
                    }
                }

                return value;
            }

            template<typename Parser>
            bool try_parse_object(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID, const Parser& _Parse)
            {
                if(_Object.get_name() != _ID)
                    return false;

                ElementObj nameObj = _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Name";
                });

                ElementObj hashObj = _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Hash";
                });

                std::string name = parse_value<std::string>(
                    nameObj,
                    [](const std::string& _Value)->std::string{return _Value;});

                std::string hash = parse_value<std::string>(
                    hashObj,
                    [](const std::string& _Value)->std::string{return _Value;});

                if(name.empty() || m_Context->does_node_exist(name, hash))
                    return false;

                next_node(_Object);

                _Parse(_Object, m_Context->next_id(name, hash));
                
                return true;
            }

            // layouts
            bool begin_panel(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_panel(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool begin_vertical_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_vertical_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool begin_horizontal_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_horizontal_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool begin_scrollarea(const Frenchie::Core::Serizliation::ElementObj& _Object);
            void end_scrollarea(const Frenchie::Core::Serizliation::ElementObj& _Object);

            // widgets
            bool push_button(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool label(const Frenchie::Core::Serizliation::ElementObj& _Object);
        };
    }
}