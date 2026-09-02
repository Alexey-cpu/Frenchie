#pragma once

// Application
#include <FrenchieImmediateUserInterfaceLayer.hpp>

// Core
#include <FrenchieCoreSerizliation.hpp>

// STL
#include <typeinfo>
#include <typeindex>
#include <filesystem>

namespace Frenchie
{
    namespace Application
    {
        class ImmediateUserInterfaceViewModel final
        {
        private:
            class Wrapper
            {
            public:
                Wrapper(){}
                virtual ~Wrapper(){}
            };

            template<typename Type>
            class Properties : public Wrapper
            {
            public:
                Properties(){}
                virtual ~Properties(){}
                std::map<std::string, Type> m_Properties;
            };

            std::map<std::type_index, std::shared_ptr<Wrapper>> m_Wrappers;

        public:
            ImmediateUserInterfaceViewModel(){}
            virtual ~ImmediateUserInterfaceViewModel(){}

            template<typename Type>
            std::map<std::string, Type>& all_of_type()
            {
                if(m_Wrappers.find(typeid(Type)) == m_Wrappers.end())
                    m_Wrappers[typeid(Type)] = std::make_shared<Properties<Type>>();
                return std::dynamic_pointer_cast<Properties<Type>>(m_Wrappers[typeid(Type)])->m_Properties;
            }

            template<typename Type>
            Type& request(const std::string& _ID)
            {
                return all_of_type<Type>()[_ID];
            }
        };

        class ImmediateUserInterfaceViewController
        {
        public:
            ImmediateUserInterfaceViewController(){}
            virtual ~ImmediateUserInterfaceViewController(){}

            virtual bool setup(std::shared_ptr<ImmediateUserInterfaceViewModel>&)   = 0;
            virtual void update(std::shared_ptr<ImmediateUserInterfaceViewModel>&)  = 0;
            virtual void destroy(std::shared_ptr<ImmediateUserInterfaceViewModel>&) = 0;
        };

        class ImmediateUserInterfaceModelViewControllerLayer : public Layer
        {
        public:
            ImmediateUserInterfaceModelViewControllerLayer(
                const std::filesystem::path&                                 _View,
                const std::shared_ptr<ImmediateUserInterfaceViewController>& _Controller = nullptr,
                const std::string&                                           _WindowName = std::string());
            virtual ~ImmediateUserInterfaceModelViewControllerLayer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void finish() override;

        private:

            std::shared_ptr<ImmediateUserInterfaceViewModel>                           m_Model;
            Frenchie::Core::Serizliation::Document                                     m_View;
            std::filesystem::path                                                      m_ViewPath;
            Frenchie::Core::Serizliation::Document::Status                             m_ViewStatus;
            std::filesystem::file_time_type                                            m_ViewLastWriteTime;
            std::shared_ptr<ImmediateUserInterfaceViewController>                      m_Controller;
            std::shared_ptr<Frenchie::Application::ImmediateUserInterfaceContextLayer> m_Context;

            // service methods
            void parse_hierarchy(const Frenchie::Core::Serizliation::ElementObj& _Object);

            template<typename Type, typename Parser>
            Type& parse_value(const Frenchie::Core::Serizliation::ElementObj& _Object, const Parser& _Parser)
            {
                Frenchie::Core::Serizliation::ElementObj sourceObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Source";});

                return
                    sourceObj.is_not_null() && !sourceObj.get_value().empty() ?
                        m_Model->request<Type>(std::string(sourceObj.get_value())) :
                            (m_Model->request<Type>(std::string(_Object.get_name())) = _Parser(_Object.get_value()));
            }

            template<typename Type>
            Type& parse_value_or_default(const Frenchie::Core::Serizliation::ElementObj& _Object, const Type& _Default)
            {
                return parse_value<Type>(
                    _Object,
                    [&_Default](const std::string_view& _Value)->Type
                    {
                        return !_Value.empty() ? Frenchie::Core::String::from_string<Type>(_Value) : _Default;
                    }
                );
            }

            gs_color& parse_value_or_default_color(const Frenchie::Core::Serizliation::ElementObj& _Object, const gs_color& _Default)
            {
                return parse_value<gs_color>(
                    _Object,
                    [&_Object, &_Default](const std::string_view& _Value)->gs_color
                    {
                        if(_Value.empty())
                            return _Default;

                        std::optional<gs_color> r, g, b, a;
                        int s = 0, t = 0;

                        for(int i = 0; i < (int)_Value.size() && _Value[i] != ']'; i++)
                        {
                            if(_Value[i] == '[') ++s;
                            if(_Value[i] != ';') continue;

                            t = i;

                            if(!r.has_value())
                                r = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], t - s));
                            else if(!g.has_value())
                                g = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], t - s));
                            else if(!b.has_value())
                                b = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], t - s));
                            else if(!a.has_value())
                                a = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], t - s));
                            
                            if(++t < (int)_Value.size())
                                s = t;
                        }

                        if(!r.has_value())
                            r = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], (int)_Value.size()));
                        else if(!g.has_value())
                            g = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], (int)_Value.size()));
                        else if(!b.has_value())
                            b = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], (int)_Value.size()));
                        else if(!a.has_value())
                            a = Frenchie::Core::String::from_string<gs_color>(std::string_view(&_Value[s], (int)_Value.size()));

                        return gs_color_rgba(
                            r.has_value() ? r.value() : 255,
                            g.has_value() ? g.value() : 255,
                            b.has_value() ? b.value() : 255,
                            a.has_value() ? a.value() : 255);
                    }
                );
            }

            Frenchie::Application::ApplicationRenderingBackendTexture& parse_value_or_default_texture(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                return parse_value<Frenchie::Application::ApplicationRenderingBackendTexture>(
                    _Object,
                    [this, &_Object](const std::string_view& _Value)->Frenchie::Application::ApplicationRenderingBackendTexture
                    {
                        std::filesystem::path path(_Value);

                        if(!std::filesystem::exists(path) || std::filesystem::is_directory(path))
                            return Frenchie::Application::ApplicationRenderingBackendTexture();

                        Frenchie::Application::ApplicationRenderingBackendTexture tex =
                            m_Model->request<Frenchie::Application::ApplicationRenderingBackendTexture>(std::string(_Object.get_name()));

                        if(!tex.is_null())
                            return tex;

                        return Frenchie::Application::ApplicationRenderingBackend::construct_texture(path.string().c_str());
                    }
                );
            }

            template<typename Parser>
            bool parse_object(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID, const Parser& _Parse, const bool _Force = false)
            {
                if(_Object.get_name() != _ID || !parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Enabled";}), true))
                    return false;

                Frenchie::Core::Serizliation::ElementObj nameObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
                Frenchie::Core::Serizliation::ElementObj hashObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
                std::string_view name = parse_value<std::string_view>(nameObj, [](const std::string_view& _Value)->std::string_view{return _Value;});
                std::string_view hash = parse_value<std::string_view>(hashObj, [](const std::string_view& _Value)->std::string_view{return _Value;});

                if(!_Force && (name.empty() || m_Context->does_node_exist(name, hash)))
                    return false;

                Frenchie::Core::Serizliation::ElementObj width         = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Width";});
                Frenchie::Core::Serizliation::ElementObj height        = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Height";});
                Frenchie::Core::Serizliation::ElementObj maximumWidth  = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "MaxWidth";});
                Frenchie::Core::Serizliation::ElementObj maximumHeight = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "MaxHeight";});
                Frenchie::Core::Serizliation::ElementObj minimumWidth  = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "MinWidth";});
                Frenchie::Core::Serizliation::ElementObj minimumHeight = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "MinHeight";});
                Frenchie::Core::Serizliation::ElementObj margin        = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Margin";});
                Frenchie::Core::Serizliation::ElementObj padding       = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Padding";});
                Frenchie::Core::Serizliation::ElementObj sameLine      = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "SameLine";});
                Frenchie::Core::Serizliation::ElementObj indent        = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Indent";});

                if(!width.get_value().empty())
                    m_Context->next_width(Frenchie::Core::String::from_string<float>(width.get_value()));

                if(!height.get_value().empty())
                    m_Context->next_height(Frenchie::Core::String::from_string<float>(height.get_value()));

                if(!maximumWidth.get_value().empty())
                    m_Context->next_maximum_width(Frenchie::Core::String::from_string<float>(maximumWidth.get_value()));

                if(!maximumHeight.get_value().empty())
                    m_Context->next_maximum_height(Frenchie::Core::String::from_string<float>(maximumHeight.get_value()));

                if(minimumWidth.is_not_null() && !minimumWidth.get_value().empty())
                    m_Context->next_minimum_width(Frenchie::Core::String::from_string<float>(minimumWidth.get_value()));

                if(!minimumHeight.get_value().empty())
                    m_Context->next_minimum_height(Frenchie::Core::String::from_string<float>(minimumHeight.get_value()));

                if(margin.is_not_null())
                {
                    Frenchie::Core::Serizliation::ElementObj top    = margin.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Top";});
                    Frenchie::Core::Serizliation::ElementObj left   = margin.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Left";});
                    Frenchie::Core::Serizliation::ElementObj right  = margin.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Right";});
                    Frenchie::Core::Serizliation::ElementObj bottom = margin.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Bottom";});

                    m_Context->next_content_margin(
                        gs_vec4f(
                            !top.get_value().empty()    ? Frenchie::Core::String::from_string<float>(top.get_value()) : 0.f,
                            !left.get_value().empty()   ? Frenchie::Core::String::from_string<float>(left.get_value()) : 0.f,
                            !right.get_value().empty()  ? Frenchie::Core::String::from_string<float>(right.get_value()) : 0.f,
                            !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(bottom.get_value()) : 0.f));
                }

                if(padding.is_not_null())
                {
                    Frenchie::Core::Serizliation::ElementObj top    = padding.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Top";});
                    Frenchie::Core::Serizliation::ElementObj left   = padding.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Left";});
                    Frenchie::Core::Serizliation::ElementObj right  = padding.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Right";});
                    Frenchie::Core::Serizliation::ElementObj bottom = padding.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Bottom";});

                    m_Context->next_content_padding(
                        gs_vec4f(
                            !top.get_value().empty()    ? Frenchie::Core::String::from_string<float>(top.get_value()) : 0.f,
                            !left.get_value().empty()   ? Frenchie::Core::String::from_string<float>(left.get_value()) : 0.f,
                            !right.get_value().empty()  ? Frenchie::Core::String::from_string<float>(right.get_value()) : 0.f,
                            !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(bottom.get_value()) : 0.f));
                }

                if(Frenchie::Core::String::from_string<bool>(sameLine.get_value()))
                    m_Context->same_line();

                if(!indent.get_value().empty())
                    m_Context->indent(Frenchie::Core::String::from_string<float>(indent.get_value()));
                
                return _Parse(_Object, m_Context->next_id(name, hash));
            }

            int parse_node_settings(const Frenchie::Core::Serizliation::ElementObj& _Object);

            // layouts
            bool begin_grid(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_grid_place(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_panel(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_scrollarea(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_vertical_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_horizontal_stack(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_menu(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_combobox(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_menubar(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_popup(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_what_is_it(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_plot(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool begin_tree_node(const Frenchie::Core::Serizliation::ElementObj& _Object);

            // widgets
            bool image(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool label(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool push_button(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool image_button(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool menu_action(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool combobox_item(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool checkbox(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool radiobutton(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool sliderbutton(const Frenchie::Core::Serizliation::ElementObj& _Object);
            
            // plotting
            bool plot_axis_x(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool plot_axis_y(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool plot_line(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool plot_pie(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool plot_vector(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool plot_line_legend(const Frenchie::Core::Serizliation::ElementObj& _Object);
            
            template<typename Type>
            bool parse_input_scalar(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
            {
                return parse_object(
                    _Object,
                    _ID,
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        if(m_Context->input_scalar<Type>(
                            _ID,

                            // value ref
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Value";}), (Type)0),

                            // minimum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Min";}), gs_tiny<Type>()),

                            // maximum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Max";}), gs_huge<Type>()),

                            // settings
                            ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter
                            | ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_StopEditOnEscape))
                        {
                            std::function<void()> action = parse_value<std::function<void()>>(
                                _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool
                                {
                                    return _Object.get_name() == "Action";
                                }),
                                [](const std::string_view& _Value)->std::function<void()>{return nullptr;});

                            if(action != nullptr)
                                action();
                        }

                        return true;
                    }
                );
            }

            bool input_scalar(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                // auxiliary lambdas
                return parse_object(
                    _Object,
                    "InputScalar",
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        Frenchie::Core::Serizliation::ElementObj typeObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Type";});

                        if(typeObj.get_value() == "float")
                            return parse_input_scalar<float>(_Object, "InputScalar");

                        if(typeObj.get_value() == "double")
                            return parse_input_scalar<double>(_Object, "InputScalar");

                        if(typeObj.get_value() == "long double")
                            return parse_input_scalar<long double>(_Object, "InputScalar");

                        if(typeObj.get_value() == "int")
                            return parse_input_scalar<int>(_Object, "InputScalar");

                        if(typeObj.get_value() == "unsigned int")
                            return parse_input_scalar<unsigned int>(_Object, "InputScalar");

                        return parse_input_scalar<float>(_Object, "InputScalar");
                    }
                );
            }

            template<typename Type>
            bool parse_input_scalar_slider(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
            {
                return parse_object(
                    _Object,
                    _ID,
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        if(m_Context->input_scalar_slider<Type>(
                            _ID,
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Value";}), (Type)0),

                            // minimum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Min";}), gs_tiny<Type>()),
                            
                            // maximum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Max";}), gs_huge<Type>()),
                            
                            // slider value delta
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Delta";}),(Type)1),

                            // settings
                            ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_ReturnTrueOnEnter
                            | ImmediateUserInterfaceInputScalarSettings_::ImmediateUserInterfaceInputScalarSettings_StopEditOnEscape))
                        {
                            std::function<void()> action = parse_value<std::function<void()>>(
                                _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool
                                {
                                    return _Object.get_name() == "Action";
                                }),
                                [](const std::string_view& _Value)->std::function<void()>{return nullptr;});

                            if(action != nullptr)
                                action();
                        }

                        return true;
                    }
                );
            }

            bool input_scalar_slider(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                return parse_object(
                    _Object,
                    "InputScalarSlider",
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        Frenchie::Core::Serizliation::ElementObj typeObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Type";});

                        if(typeObj.get_value() == "float")
                            return parse_input_scalar_slider<float>(_Object, "InputScalarSlider");

                        if(typeObj.get_value() == "double")
                            return parse_input_scalar_slider<double>(_Object, "InputScalarSlider");

                        if(typeObj.get_value() == "long double")
                            return parse_input_scalar_slider<long double>(_Object, "InputScalarSlider");

                        if(typeObj.get_value() == "int")
                            return parse_input_scalar_slider<int>(_Object, "InputScalarSlider");

                        if(typeObj.get_value() == "unsigned int")
                            return parse_input_scalar_slider<unsigned int>(_Object, "InputScalarSlider");

                        return parse_input_scalar_slider<float>(_Object, "InputScalarSlider");
                    }
                );
            }

            template<typename Type>
            bool parse_progressbar_default(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
            {
                return parse_object(
                    _Object,
                    _ID,
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        m_Context->progressbar_default<Type>(
                            _ID,

                            // value reference
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Value";}), (Type)0),

                            // minimum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Min";}), gs_tiny<Type>()),
                            
                            // maximum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Max";}), gs_huge<Type>()));

                        return true;
                    }
                );
            };

            bool progressbar_default(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                return parse_object(
                    _Object,
                    "ProgressBarDefault",
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        Frenchie::Core::Serizliation::ElementObj typeObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Type";});

                        if(typeObj.get_value() == "float")
                            return parse_progressbar_default<float>(_Object, "ProgressBarDefault");

                        if(typeObj.get_value() == "double")
                            return parse_progressbar_default<double>(_Object, "ProgressBarDefault");

                        if(typeObj.get_value() == "long double")
                            return parse_progressbar_default<long double>(_Object, "ProgressBarDefault");

                        if(typeObj.get_value() == "int")
                            return parse_progressbar_default<int>(_Object, "ProgressBarDefault");

                        if(typeObj.get_value() == "unsigned int")
                            return parse_progressbar_default<unsigned int>(_Object, "ProgressBarDefault");

                        return parse_progressbar_default<float>(_Object, "ProgressBarDefault");
                    }
                );
            }

            template<typename Type>
            auto parse_progressbar_circular(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
            {
                return parse_object(
                    _Object,
                    _ID,
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        m_Context->progressbar_circular(
                            _ID,

                            // value reference
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Value";}), (Type)0),

                            // minimum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Min";}), gs_tiny<Type>()),

                            // maximum value
                            parse_value_or_default<Type>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object)->bool{return _Object.get_name() == "Max";}), gs_huge<Type>()));

                        return true;
                    }
                );
            }

            bool progressbar_circular(const Frenchie::Core::Serizliation::ElementObj& _Object)
            {
                return parse_object(
                    _Object,
                    "ProgressBarCircular",
                    [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
                    {
                        Frenchie::Core::Serizliation::ElementObj typeObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Type";});

                        if(typeObj.get_value() == "float")
                            return parse_progressbar_circular<float>(_Object, "ProgressBarCircular");

                        if(typeObj.get_value() == "double")
                            return parse_progressbar_circular<double>(_Object, "ProgressBarCircular");

                        if(typeObj.get_value() == "long double")
                            return parse_progressbar_circular<long double>(_Object, "ProgressBarCircular");

                        if(typeObj.get_value() == "int")
                            return parse_progressbar_circular<int>(_Object, "ProgressBarCircular");

                        if(typeObj.get_value() == "unsigned int")
                            return parse_progressbar_circular<unsigned int>(_Object, "ProgressBarCircular");

                        return parse_progressbar_circular<float>(_Object, "ProgressBarCircular");
                    }
                );
            }

            bool input_color(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_string(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID, bool _SingleLine);
            bool input_string_singleline(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool input_string_multiline(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool color_picker_rgba(const Frenchie::Core::Serizliation::ElementObj& _Object);
            bool color_picker_hsva(const Frenchie::Core::Serizliation::ElementObj& _Object);
        };
    }
}