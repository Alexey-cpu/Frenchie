// Application
#include <FrenchieImmediateUserInterfaceModelViewControllerLayer.hpp>

// Core
#include <FrenchieCoreSerizliationXML.hpp>
#include <FrenchieCoreSerizliationJSON.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::String;
using namespace Frenchie::Core::Serizliation;
using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        namespace MVC
        {
            Frenchie::Core::Serizliation::Document::Status read_file(Frenchie::Core::Serizliation::Document& _Document, const std::filesystem::path& _File)
            {
                if(!std::filesystem::exists(_File) || std::filesystem::is_directory(_File))
                {
                    return Frenchie::Core::Serizliation::Document::Status(
                        false,
                        std::string("file does not exists ")
                            .append(Frenchie::Core::String::convert_utf32_to_utf8(_File.u32string())));
                }

                std::string extention = Frenchie::Core::String::utf8_to_lower(
                    Frenchie::Core::String::convert_utf32_to_utf8(
                        _File.extension().u32string()));

                if(extention == ".xml")
                    return _Document.read_file<Frenchie::Core::Serizliation::XML::Parser>(Frenchie::Core::String::convert_utf32_to_utf8(_File.u32string()));
                else if(extention == ".json")
                    return _Document.read_file<Frenchie::Core::Serizliation::JSON::Parser>(Frenchie::Core::String::convert_utf32_to_utf8(_File.u32string()));
                else
                    return Frenchie::Core::Serizliation::Document::Status(false, std::string("unknown file format ").append(extention));
            }
        }
    }
}

// ImmediateUserInterfaceModelLayer
ImmediateUserInterfaceModelViewControllerLayer::ImmediateUserInterfaceModelViewControllerLayer(
    const std::filesystem::path&                                 _View,
    const std::shared_ptr<ImmediateUserInterfaceViewController>& _Controller,
    const std::string&                                           _Name) :
    Layer((_Name.empty() ? _View.stem().string() : _Name)),
    m_ViewPath(_View),
    m_Controller(_Controller),
    m_Model(std::make_shared<ImmediateUserInterfaceViewModel>()){}

ImmediateUserInterfaceModelViewControllerLayer::~ImmediateUserInterfaceModelViewControllerLayer(){}

bool ImmediateUserInterfaceModelViewControllerLayer::awake()
{
    if(m_Context == nullptr)
        m_Context = Frenchie::Application::App::push_layer<ImmediateUserInterfaceContextLayer>();

    if((m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
        m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);

    if(m_Controller != nullptr)
        m_Controller->setup(m_Model);

    return m_Context != nullptr;
}

void ImmediateUserInterfaceModelViewControllerLayer::frame_start()
{
    if(std::filesystem::exists(m_ViewPath) && !std::filesystem::is_directory(m_ViewPath) && std::filesystem::last_write_time(m_ViewPath) != m_ViewLastWriteTime && (m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
        m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);

    if(m_Controller != nullptr)
        m_Controller->update(m_Model);
}

void ImmediateUserInterfaceModelViewControllerLayer::frame_update()
{
    if(m_Context->does_node_exist(get_name(), get_name()))
    {
        close();
        return;
    }

    if(m_Context->begin_window(
        m_Context->next_id(get_name(), get_name()),
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults,
        &m_Opened))
    {
        if(m_ViewStatus)
            parse_hierarchy(m_View.get_root());
        else
            m_Context->label(m_Context->next_id("Error"), m_ViewStatus.m_Message);

        m_Context->end_window();
    }
}

void ImmediateUserInterfaceModelViewControllerLayer::finish()
{
    if(m_Controller != nullptr)
        m_Controller->destroy(m_Model);
}

void ImmediateUserInterfaceModelViewControllerLayer::parse_hierarchy(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(_Object.get_name().empty())
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        return;
    }

    // parse widgets
    if(image(_Object)) return;
    if(label(_Object)) return;
    if(push_button(_Object)) return;
    if(image_button(_Object)) return;
    if(input_scalar(_Object)) return;
    if(input_scalar_slider(_Object)) return;
    if(progressbar_default(_Object)) return;
    if(progressbar_circular(_Object)) return;
    if(input_color(_Object)) return;
    if(color_picker_rgba(_Object)) return;
    if(color_picker_hsva(_Object)) return;
    if(menu_action(_Object)) return;
    if(input_string_singleline(_Object)) return;
    if(input_string_multiline(_Object)) return;
    if(combobox_item(_Object)) return;
    if(plot_axis_x(_Object))return;
    if(plot_axis_y(_Object))return;
    if(plot_line(_Object))return;
    if(plot_pie(_Object))return;
    if(plot_vector(_Object))return;
    if(plot_line_legend(_Object))return;
    if(checkbox(_Object))return;
    if(radiobutton(_Object))return;
    if(sliderbutton(_Object))return;

    // parse hierarchies
    if(begin_panel(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_panel();
        return;
    }

    if(begin_scrollarea(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_scrollarea();
        return;
    }

    if(begin_vertical_stack(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_vertical_stack();
        return;
    }

    if(begin_horizontal_stack(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_horizontal_stack();
        return;
    }

    if(begin_menu(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_menu();
        return;
    }
    
    if(begin_grid(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_grid();
        return;
    }

    if(begin_grid_place(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_grid_place();
        return;
    }

    if(begin_combobox(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_combobox();
        return;
    }

    if(begin_menubar(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_menubar();
        return;
    }

    if(begin_popup(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_popup();
        return;
    }

    if(begin_what_is_it(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_popup();
        return;
    }

    if(begin_plot(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_plot();
        return;
    }

    if(begin_tree_node(_Object))
    {
        for(auto child : _Object)
            parse_hierarchy(child);
        m_Context->end_tree_node();
        return;
    }
}

int ImmediateUserInterfaceModelViewControllerLayer::parse_node_settings(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(_Object.get_name() != "Settings")
        return ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults;

    int settings = 0;

    if(parse_value_or_default<bool>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Movable";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;

    if(parse_value_or_default<bool>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Resizable";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable;
    
    std::string verticalAlignment = parse_value_or_default<std::string>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "VerticalAlignment";}), std::string());
    if(Frenchie::Core::String::utf8_to_lower(verticalAlignment) == "top")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
    else if(Frenchie::Core::String::utf8_to_lower(verticalAlignment) == "center")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
    else if(Frenchie::Core::String::utf8_to_lower(verticalAlignment) == "bottom")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;

    std::string horizontalAlignment = parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HorizontalAlignment";}), std::string());
    if(Frenchie::Core::String::utf8_to_lower(horizontalAlignment) == "left")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
    else if(Frenchie::Core::String::utf8_to_lower(horizontalAlignment) == "center")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
    else if(Frenchie::Core::String::utf8_to_lower(horizontalAlignment) == "right")
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;

    if(parse_value_or_default<bool>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ResizeToContentsVertically";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;
    
    if(parse_value_or_default<bool>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ResizeToContentsHorizontally";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally;
    
    if(parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "LayoutClampWhenNoChildren";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_LayoutClampWhenNoChildren;

    ElementObj verticalScrollBar = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "VerticalScrollBar";});

    if(verticalScrollBar.is_not_null())
    {
        std::string visibility = parse_value_or_default<std::string>(verticalScrollBar.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Visibility";}), std::string());

        if(Frenchie::Core::String::utf8_to_lower(visibility) == "never")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar;
        else if(Frenchie::Core::String::utf8_to_lower(visibility) == "always")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar;
        else if((Frenchie::Core::String::utf8_to_lower(visibility) == "auto" || Frenchie::Core::String::utf8_to_lower(visibility) == "adaptive"))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar;

        if(parse_value_or_default<bool>(verticalScrollBar.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MouseWheelAdjustment";}), false))
            settings |= ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment;

        if(parse_value_or_default<bool>(verticalScrollBar.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ArrowKeysAdjustment";}), false))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment;
    }

    ElementObj horizontalScrollBar = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HorizontalScrollBar";});

    if(horizontalScrollBar.is_not_null())
    {
        std::string visibility = parse_value_or_default<std::string>(verticalScrollBar.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Visibility";}), std::string());

        if(Frenchie::Core::String::utf8_to_lower(visibility) == "never")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar;
        else if(Frenchie::Core::String::utf8_to_lower(visibility) == "always")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar;
        else if((Frenchie::Core::String::utf8_to_lower(visibility) == "auto" || Frenchie::Core::String::utf8_to_lower(visibility) == "adaptive"))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar;

        if(parse_value_or_default(horizontalScrollBar.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ArrowKeysAdjustment";}), false))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment;
    }

    if(parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "PlotFitXAxis";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotFitXAxis;

    if(parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "PlotFitYAxis";}), false))
        settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_PlotFitYAxis;

    return settings <= 0 ? ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults : settings;
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_grid(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Grid",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            return m_Context->begin_grid(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_grid_place(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "GridPlace",
        [this](const ElementObj& _Object, const std::string&)->bool
        {
            int row = parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Row";}), -1);
            int col = parse_value_or_default(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Col";}), -1);
            
            if(row < 0 || col < 0 || m_Context->does_node_exist(Frenchie::Core::String::format("Place-%d-%d", row, col))) return false;

            return m_Context->begin_grid_place(
                row,
                col,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        },
        true
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_panel(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Panel",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            return m_Context->begin_panel(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_scrollarea(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ScrollArea",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            return m_Context->begin_scrollarea(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_vertical_stack(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "VerticalStack",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            return m_Context->begin_vertical_stack(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_horizontal_stack(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "HorizontalStack",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            return m_Context->begin_horizontal_stack(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";})));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_menu(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Menu",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            return m_Context->begin_menu(_ID);
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_combobox(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Combobox",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            return m_Context->begin_combobox(
                _ID,
                parse_value_or_default<std::string>(
                    _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "PreviewText";}),
                    std::string()
                )
            );
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_menubar(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "MenuBar",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
        {
            return m_Context->begin_menubar(_ID);
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_popup(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Popup",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            return m_Context->begin_popup(
                _ID,
                m_Context->is_current_node_mouse_clicked(ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonRight, m_Context->get_rendering_stack_top()) ||
                m_Context->is_current_node_mouse_clicked(ApplicationPlatformBackendMouseButton::Button::ApplicationPlatformBackendMouseButtonRight, m_Context->get_rendered_stack_top())
            );
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_what_is_it(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "WhatIsIt",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            return
                (m_Context->is_current_node_mouse_hovered(m_Context->get_rendering_stack_top()) ||
                m_Context->is_current_node_mouse_hovered(m_Context->get_rendered_stack_top())) && m_Context->begin_popup(_ID, true);
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_plot(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Plot",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
        {
            if(!m_Context->begin_plot(_ID, parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";}))))
                return false;

            Frenchie::Core::Serizliation::ElementObj nameObj =
                _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Name";});

            if(nameObj.is_not_null() && !nameObj.get_name().empty())
                m_Model->request<ImmediateUserInterfaceNode*>(std::string(nameObj.get_value())) = m_Context->get_rendering_stack_top();

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_tree_node(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "TreeNode",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)
        {
            int settings = 0;

            Frenchie::Core::Serizliation::ElementObj settingsObj = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Settings";});
            
            Frenchie::Core::Serizliation::ElementObj openOnClickObj =
                settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "OpenOnClick";});

            if(parse_value_or_default(openOnClickObj, false))
                settings |= ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_OpenOnClick;

            Frenchie::Core::Serizliation::ElementObj openOnDoubleClickObj =
                settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "OpenOnDoubleClick";});

            if(parse_value_or_default(openOnDoubleClickObj, false))
                settings |= ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_OpenOnDoubleClick;

            Frenchie::Core::Serizliation::ElementObj renderConnectionLinesObj =
                settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "RenderConnectionLines";});

            if(parse_value_or_default(renderConnectionLinesObj, false))
                settings |= ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_RenderConnectionLines;

            if(settings <= 0)
                settings = ImmediateUserInterfaceTreeNodeSettings_::ImmediateUserInterfaceTreeNodeSettings_Defaults;

            return m_Context->begin_tree_node(
                _ID,
                settings,

                // open state texture
                parse_value<ApplicationRenderingBackendTexture>(
                    _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "TextureOpen";}),
                    [](const std::string&){return ApplicationRenderingBackendTexture();}),

                // close state texture
                parse_value<ApplicationRenderingBackendTexture>(
                    _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "TextureClose";}),
                    [](const std::string&){return ApplicationRenderingBackendTexture();}));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::image(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Image",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
        {
            m_Context->image(
                _ID,

                // color mask
                parse_value_or_default_color(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Color";}), gs_color_rgb(255, 255, 255)),

                // image
                parse_value<ApplicationRenderingBackendTexture>(
                    _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Texture";}),
                    [](const std::string&){return ApplicationRenderingBackendTexture();}));

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::label(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Label",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            // parse text settings
            ElementObj settingsObj  = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";});
            ElementObj alignmentObj = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Alignment";});

            int settings = 0;

            if(alignmentObj.is_not_null() && !alignmentObj.get_value().empty())
            {
                settings |= parse_value<int>(
                    alignmentObj,
                    [](const std::string& _Value)
                    {
                        std::string value = Frenchie::Core::String::utf8_to_lower(_Value);

                        if(value == "left")
                            return ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignLeft;

                        if(value == "center")
                            return ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignCenter;

                        if(value == "right")
                            return ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_AlignRight;

                        return ImmediateUserInterfaceLabelSettings_::ImmediateUserInterfaceLabelSettings_None;
                    }
                );
            }

            m_Context->label(
                _ID,
                
                // text
                parse_value_or_default<std::string>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Text";}), std::string()),
                
                // settings
                settings,
                
                // maximum symbols count
                parse_value_or_default<int>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "MaximumSymbolsCount";}), gs_huge<int>()
            ));

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::push_button(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PushButton",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::function<void(ImmediateUserInterfaceContextLayer*)> callback = parse_value<std::function<void(ImmediateUserInterfaceContextLayer*)>>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Action";
                }),
                [](const std::string& _Value)->std::function<void(ImmediateUserInterfaceContextLayer*)>{return nullptr;});

            if(m_Context->push_button(_ID) && callback != nullptr)
                callback(m_Context.get());

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::image_button(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ImageButton",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::function<void(ImmediateUserInterfaceContextLayer*)> callback = parse_value<std::function<void(ImmediateUserInterfaceContextLayer*)>>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Action";
                }),
                [](const std::string& _Value)->std::function<void(ImmediateUserInterfaceContextLayer*)>{return nullptr;});

            gs_color color = parse_value_or_default_color(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Color";
                }),
                gs_color_rgb(255, 255, 255)
            );

            ApplicationRenderingBackendTexture texture = parse_value<ApplicationRenderingBackendTexture>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Texture";
                }),
                [](const std::string&)
                {
                    return ApplicationRenderingBackendTexture();
                });

            if(m_Context->image_button(_ID, color, texture) && callback != nullptr)
                callback(m_Context.get());

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::menu_action(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "MenuAction",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::function<void(ImmediateUserInterfaceContextLayer*)> callback = parse_value<std::function<void(ImmediateUserInterfaceContextLayer*)>>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Action";
                }),
                [](const std::string& _Value)->std::function<void(ImmediateUserInterfaceContextLayer*)>{return nullptr;});

            if(m_Context->menu_action(_ID) && callback != nullptr)
                callback(m_Context.get());

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::combobox_item(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ComboboxItem",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::function<void(ImmediateUserInterfaceContextLayer*)> callback = parse_value<std::function<void(ImmediateUserInterfaceContextLayer*)>>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Action";
                }),
                [](const std::string& _Value)->std::function<void(ImmediateUserInterfaceContextLayer*)>{return nullptr;});

            if(m_Context->combobox_item(_ID) && callback != nullptr)
                callback(m_Context.get());

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::checkbox(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "CheckBox",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
        {
            int settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkbox;

            if(parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checkable";}), true))
                settings |= ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable;

            m_Context->check_button(
                _ID,
                parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checked";}), true),
                settings);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::radiobutton(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "RadioButton",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
        {
            int settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_RadioButton;

            if(parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checkable";}), true))
                settings |= ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable;

            m_Context->check_button(
                _ID,
                parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checked";}), true),
                settings);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::sliderbutton(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "SliderButton",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
        {
            int settings = ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_SliderButton;

            if(parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checkable";}), true))
                settings |= ImmediateUserInterfaceCheckButtonSettings_::ImmediateUserInterfaceCheckButtonSettings_Checkable;

            m_Context->check_button(
                _ID,
                parse_value_or_default<bool>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Checked";}), true),
                settings);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_axis_x(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "XAxis",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            Frenchie::Core::Serizliation::ElementObj settingsObj   = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Settings";});
            Frenchie::Core::Serizliation::ElementObj zoomableObj   = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Zoom";});
            Frenchie::Core::Serizliation::ElementObj scrollableObj = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Scroll";});

            int settings = 0;

            if(parse_value_or_default<bool>(zoomableObj, false))
                settings |= ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Zoomable;
            
            if(parse_value_or_default<bool>(scrollableObj, false))
                settings |= ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Scrollable;

            if(settings <= 0)
                settings = ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Defaults;

            float min   = parse_value_or_default<float>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Min";}), -10.f);
            float max   = parse_value_or_default<float>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Max";}), +10.f);
            int   ticks = parse_value_or_default<int>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Ticks";}), 10);
            m_Context->plot_axis_x(_ID, min, max, ticks, settings);
            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_axis_y(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "YAxis",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            Frenchie::Core::Serizliation::ElementObj settingsObj   = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Settings";});
            Frenchie::Core::Serizliation::ElementObj zoomableObj   = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Zoom";});
            Frenchie::Core::Serizliation::ElementObj scrollableObj = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Scroll";});

            int settings = 0;

            if(parse_value_or_default<bool>(zoomableObj, false))
                settings |= ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Zoomable;
            
            if(parse_value_or_default<bool>(scrollableObj, false))
                settings |= ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Scrollable;

            if(settings <= 0)
                settings = ImmediateUserInterfacePlotLineAxisSettings_::ImmediateUserInterfacePlotLineAxisSettings_Defaults;

            float min   = parse_value_or_default<float>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Min";}), -10.f);
            float max   = parse_value_or_default<float>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Max";}), +10.f);
            int   ticks = parse_value_or_default<int>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Ticks";}), 10);
            m_Context->plot_axis_y(_ID, min, max, ticks, settings);
            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_line(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PlotLine",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            float*   xValues   = parse_value<float*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "X";}), [](const std::string&){return nullptr;});
            float*   yValues   = parse_value<float*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Y";}), [](const std::string&){return nullptr;});
            int      xyCount   = parse_value_or_default<int>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Count";}), 0);
            gs_color color     = parse_value_or_default_color(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Color";}), gs_color_rgb(255, 255, 255));
            float    lineWidth = parse_value_or_default<float>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "LineWidth";}), 8.f);

            Frenchie::Core::Serizliation::ElementObj settingsObj             = _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Settings";});
            Frenchie::Core::Serizliation::ElementObj plotTypeObj             = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Type";});
            Frenchie::Core::Serizliation::ElementObj markersTypeObj          = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Markers";});
            Frenchie::Core::Serizliation::ElementObj openMarkersTypeObj      = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "OpenMarkers";});
            Frenchie::Core::Serizliation::ElementObj highlightAxisOnHoverObj = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "HighlightOnAxisHover";});
            Frenchie::Core::Serizliation::ElementObj renderLabelsOnHoverObj  = settingsObj.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "RenderLabelsOnHover";});

            int settings = 0;
            
            std::string plotType = Frenchie::Core::String::utf8_to_lower(parse_value_or_default<std::string>(plotTypeObj, std::string("lines")));

            if(plotType == "lines")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsLines;
            else if(plotType == "stems")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsStems;
            else if(plotType == "points")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsPoints;
            else if(plotType == "rectangles")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsRectangles;
            else if(plotType == "areas")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderAsConvexAreas;

            std::string markersType = Frenchie::Core::String::utf8_to_lower(parse_value_or_default<std::string>(markersTypeObj, std::string()));

            if(markersType == "points")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersPoints;
            else if(markersType == "triangles")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersTriangles;
            else if(markersType == "rectangles")
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_MarkersRectangles;

            if(parse_value_or_default<bool>(renderLabelsOnHoverObj, false))
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_RenderLabelsOnHover;

            if(parse_value_or_default<bool>(highlightAxisOnHoverObj, false))
                settings |= ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_HighlightOnAxisHover;

            if(settings <= 0)
                settings = ImmediateUserInterfacePlotLineSettings_::ImmediateUserInterfacePlotLineSettings_Defaults;

            if(xValues == nullptr || yValues == nullptr || xyCount <= 0)
            {
                m_Model->request<std::optional<gs_vec4f>>(_ID).reset();
                return false;
            }

            m_Model->request<std::optional<gs_vec4f>>(_ID) =
                m_Context->plot_line(_ID, xValues, yValues, xyCount, color, lineWidth, settings, m_Model->request<std::optional<gs_vec4f>>(_ID));

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_pie(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PlotPie",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::string* names  = parse_value<std::string*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Names";}), [](const std::string&){return nullptr;});
            float*       values = parse_value<float*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Values";}), [](const std::string&){return nullptr;});
            gs_color*    colors = parse_value<gs_color*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Colors";}), [](const std::string&){return nullptr;});
            int          count  = parse_value_or_default<int>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Count";}), 0);
            
            m_Context->plot_pie(names, values, colors, count);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_vector(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PlotVectors",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            std::string* names  = parse_value<std::string*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Names";}), [](const std::string&){return nullptr;});
            gs_vec4f*    values = parse_value<gs_vec4f*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Values";}), [](const std::string&){return nullptr;});
            gs_color*    colors = parse_value<gs_color*>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Colors";}), [](const std::string&){return nullptr;});
            int          count  = parse_value_or_default<int>(_Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Count";}), 0);
            
            m_Context->plot_vector(names, values, colors, count);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::plot_line_legend(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PlotLegend",
        [this](const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID)->bool
        {
            m_Context->plot_legend(
                _ID,
                parse_value<ImmediateUserInterfaceNode*>(
                    _Object.find_node([](const Frenchie::Core::Serizliation::ElementObj& _Object){return _Object.get_name() == "Plot";}),
                    [](const std::string&)
                    {
                        return nullptr;
                    }
                )
            );

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::input_color(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "InputColor",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            // parse settings
            int settings = 0;

            ElementObj settingsObj      = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";});
            ElementObj editRGB          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "RGB";});
            ElementObj editHSV          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSV";});
            ElementObj editHSL          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSL";});
            ElementObj editAlpha        = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Alpha";});
            ElementObj colorButton      = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Preview";});
            ElementObj colorDragAndDrop = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "DragDrop";});

            if(parse_value_or_default<bool>(editRGB, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(parse_value_or_default<bool>(editHSV, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(parse_value_or_default<bool>(editHSL, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(parse_value_or_default<bool>(editAlpha, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(parse_value_or_default<bool>(colorButton, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(parse_value_or_default<bool>(colorDragAndDrop, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->input_color(
                _ID,
                parse_value_or_default_color(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Color";}), gs_color_rgb(255, 255, 255)),
                settings);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::input_string(const Frenchie::Core::Serizliation::ElementObj& _Object, const std::string& _ID, bool _SingleLine)
{
    return parse_object(
        _Object,
        _ID,
        [this, &_SingleLine](const ElementObj& _Object, const std::string& _ID)->bool
        {
            ElementObj Settings = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";});

            // parse text settings
            ElementObj NoInput = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "NoInput";});
            ElementObj Password = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Password";});
            ElementObj NoClipboard = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "NoClipboard";});
            ElementObj NoSelection = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "NoSelection";});
            ElementObj ReturnTrueOnEnter = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ReturnTrueOnEnter";});
            ElementObj ReturnTrueOnEdit = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "ReturnTrueOnEdit";});
            ElementObj StopEditOnEscape = Settings.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "StopEditOnEscape";});

            int settings = 0;

            if(parse_value_or_default<bool>(NoInput, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoInput;

            if(parse_value_or_default<bool>(Password, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_Password;

            if(parse_value_or_default<bool>(NoClipboard, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoClipboard;

            if(parse_value_or_default<bool>(NoSelection, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_NoSelection;

            if(parse_value_or_default<bool>(ReturnTrueOnEnter, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter;

            if(parse_value_or_default<bool>(ReturnTrueOnEdit, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEdit;

            if(parse_value_or_default<bool>(StopEditOnEscape, false))
                settings |= ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape;

            if(settings <= 0)
            {
                if(_SingleLine)
                {
                    settings =
                        ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape
                        | ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_ReturnTrueOnEnter;
                }
                else
                {
                    settings =
                        ImmediateUserInterfaceInputStringSettings_::ImmediateUserInterfaceInputStringSettings_StopEditOnEscape;
                }
            }

            typedef bool (*inputTextFilter)(const std::string &);

            bool edited = false;

            if(_SingleLine)
            {
                edited = m_Context->input_string_singleline(
                                _ID,
                                parse_value_or_default<std::string>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Text";}), std::string()),
                                settings,
                                parse_value<inputTextFilter>(
                                    _Object.find_node([](const ElementObj& _Object)->bool
                                    {
                                        return _Object.get_name() == "Filter";
                                    }),
                                    [](const std::string& )
                                    {
                                        return nullptr;
                                    }));
            }
            else
            {
                edited = m_Context->input_string_multiline(
                                _ID,
                                parse_value_or_default<std::string>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Text";}), std::string()),
                                settings,
                                parse_value<inputTextFilter>(
                                    _Object.find_node([](const ElementObj& _Object)->bool
                                    {
                                        return _Object.get_name() == "Filter";
                                    }),
                                    [](const std::string& )
                                    {
                                        return nullptr;
                                    }));
            }

            if(edited)
            {
                std::function<void(ImmediateUserInterfaceContextLayer*)> callback = parse_value<std::function<void(ImmediateUserInterfaceContextLayer*)>>(
                    _Object.find_node([](const ElementObj& _Object)->bool
                    {
                        return _Object.get_name() == "Action";
                    }),
                    [](const std::string& _Value)->std::function<void(ImmediateUserInterfaceContextLayer*)>{return nullptr;});

                if(callback != nullptr)
                    callback(m_Context.get());
            }

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::input_string_singleline(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return input_string(_Object, "InputStringSingleLine", true);
}

bool ImmediateUserInterfaceModelViewControllerLayer::input_string_multiline(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return input_string(_Object, "InputStringMultiline", false);
}

bool ImmediateUserInterfaceModelViewControllerLayer::color_picker_rgba(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ColorPickerRGBA",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            // parse settings
            int settings = 0;

            ElementObj settingsObj      = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";});
            ElementObj editRGB          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "RGB";});
            ElementObj editHSV          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSV";});
            ElementObj editHSL          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSL";});
            ElementObj editAlpha        = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Alpha";});
            ElementObj colorButton      = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Preview";});
            ElementObj colorDragAndDrop = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "DragDrop";});

            if(parse_value_or_default<bool>(editRGB, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(parse_value_or_default<bool>(editHSV, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(parse_value_or_default<bool>(editHSL, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(parse_value_or_default<bool>(editAlpha, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(parse_value_or_default<bool>(colorButton, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(parse_value_or_default<bool>(colorDragAndDrop, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->color_picker_rgba(
                _ID,
                parse_value_or_default_color(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Color";}), gs_color_rgb(255, 255, 255)),
                settings);

            return true;
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::color_picker_hsva(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ColorPickerHSVA",
        [this](const ElementObj& _Object, const std::string& _ID)->bool
        {
            // parse settings
            int settings = 0;

            ElementObj settingsObj      = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Settings";});
            ElementObj editRGB          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "RGB";});
            ElementObj editHSV          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSV";});
            ElementObj editHSL          = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "HSL";});
            ElementObj editAlpha        = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Alpha";});
            ElementObj colorButton      = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Preview";});
            ElementObj colorDragAndDrop = settingsObj.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "DragDrop";});

            if(parse_value_or_default<bool>(editRGB, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(parse_value_or_default<bool>(editHSV, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(parse_value_or_default<bool>(editHSL, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(parse_value_or_default<bool>(editAlpha, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(parse_value_or_default<bool>(colorButton, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(parse_value_or_default<bool>(colorDragAndDrop, false))
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->color_picker_hsva(
                _ID,
                parse_value_or_default_color(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Color";}), gs_color_rgb(255, 255, 255)),
                settings);

            return true;
        }
    );
}