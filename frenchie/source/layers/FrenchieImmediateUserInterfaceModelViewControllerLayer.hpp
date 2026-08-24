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
            template<typename Type, typename Default>
            Type parse_value(const Frenchie::Core::Serizliation::ElementObj& _Object, const Default& _Default)
            {
                Type value = _Default(std::string(_Object.get_value()));

                if(_Object.empty())
                    return value;

                ElementObj sourceObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Source";});

                if(!sourceObj.get_value().empty() && m_Model.find(std::string(sourceObj.get_value())) !=  m_Model.end())
                {
                    try
                    {
                        return std::any_cast<Type>(m_Model[std::string(sourceObj.get_value())]);
                    }
                    catch(...)
                    {
                    }
                }

                return value;
            }

            template<typename Type>
            Type& parse_reference(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                ElementObj sourceObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Source";});

                if(!sourceObj.get_value().empty() && m_Model.find(std::string(sourceObj.get_value())) != m_Model.end())
                {
                    try
                    {
                        return std::any_cast<std::reference_wrapper<Type>>(m_Model[std::string(sourceObj.get_value())]).get();
                    }
                    catch(...)
                    {
                    }
                }
                
                static Type defaultValue = Type();
                m_Model[std::string(sourceObj.get_value())] = std::reference_wrapper<Type>(defaultValue);
                return defaultValue;
            }

            template<typename Type>
            void save_value(const Frenchie::Core::Serizliation::ElementObj& _Object, const Type& _Value)
            {
                ElementObj sourceObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Source";});

                if(sourceObj.is_not_null() && !sourceObj.get_value().empty())
                    m_Model[std::string(sourceObj.get_value())] = _Value;
            }

            template<typename Parser>
            bool parse_object(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID, const Parser& _Parse)
            {
                if(_Object.get_name() != _ID)
                    return false;

                ElementObj  nameObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
                ElementObj  hashObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
                std::string name    = parse_value<std::string>(nameObj, [](const std::string& _Value)->std::string{return _Value;});
                std::string hash    = parse_value<std::string>(hashObj, [](const std::string& _Value)->std::string{return _Value;});

                if(name.empty() || m_Context->does_node_exist(name, hash))
                    return false;

                ElementObj width         = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Width";});
                ElementObj height        = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Height";});
                ElementObj maximumWidth  = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MaxWidth";});
                ElementObj maximumHeight = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MaxHeight";});
                ElementObj minimumWidth  = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MinWidth";});
                ElementObj minimumHeight = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MinHeight";});
                ElementObj margin        = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Margin";});
                ElementObj padding       = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Padding";});

                if(width.is_not_null() && !width.get_value().empty())
                    m_Context->next_width(Frenchie::Core::String::from_string<float>(std::string(width.get_value())));

                if(height.is_not_null() && !height.get_value().empty())
                    m_Context->next_height(Frenchie::Core::String::from_string<float>(std::string(height.get_value())));

                if(maximumWidth.is_not_null() && !maximumWidth.get_value().empty())
                    m_Context->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumWidth.get_value())));

                if(maximumHeight.is_not_null() && !maximumHeight.get_value().empty())
                    m_Context->next_maximum_height(Frenchie::Core::String::from_string<float>(std::string(maximumHeight.get_value())));

                if(minimumWidth.is_not_null() && !minimumWidth.get_value().empty())
                    m_Context->next_minimum_width(Frenchie::Core::String::from_string<float>(std::string(minimumWidth.get_value())));

                if(minimumHeight.is_not_null() && !minimumHeight.get_value().empty())
                    m_Context->next_minimum_height(Frenchie::Core::String::from_string<float>(std::string(minimumHeight.get_value())));

                if(margin.is_not_null())
                {
                    ElementObj top    = margin.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Top";});
                    ElementObj left   = margin.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Left";});
                    ElementObj right  = margin.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Right";});
                    ElementObj bottom = margin.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Bottom";});

                    m_Context->next_content_margin(
                        gs_vec4f(
                            !top.get_value().empty()    ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                            !left.get_value().empty()   ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                            !right.get_value().empty()  ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                            !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
                }

                if(padding.is_not_null())
                {
                    ElementObj top    = padding.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Top";});
                    ElementObj left   = padding.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Left";});
                    ElementObj right  = padding.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Right";});
                    ElementObj bottom = padding.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Bottom";});

                    m_Context->next_content_padding(
                        gs_vec4f(
                            !top.get_value().empty()    ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                            !left.get_value().empty()   ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                            !right.get_value().empty()  ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                            !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
                }

                _Parse(_Object, m_Context->next_id(name, hash));
                
                return true;
            }

            int parse_node_settings(const Frenchie::Core::Serizliation::ElementObj& _Object);

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
            bool label(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool push_button(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_scalar_float(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_scalar_integer(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_scalar_slider_float(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_scalar_slider_integer(const Frenchie::Core::Serizliation::ElementObj& _Object);
            
            bool progressbar_default_float(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool progressbar_default_integer(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool progressbar_circular_float(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool progressbar_circular_integer(const Frenchie::Core::Serizliation::ElementObj& _Object);

            bool input_color(const Frenchie::Core::Serizliation::ElementObj& _Object);
        };
    }
}