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
    const std::filesystem::path&                             _View,
    const std::shared_ptr<ImmediateUserInterfaceViewController>& _Controller) :
    Layer(STRINGIFY(ImmediateUserInterfaceModelViewControllerLayer)),
    m_ViewPath(_View),
    m_Controller(_Controller){}

ImmediateUserInterfaceModelViewControllerLayer::~ImmediateUserInterfaceModelViewControllerLayer(){}

bool ImmediateUserInterfaceModelViewControllerLayer::awake()
{
    if(m_Context == nullptr)
        m_Context = Frenchie::Application::Application::push_layer<ImmediateUserInterfaceContextLayer>();

    if((m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
        m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);

    return m_Context != nullptr;
}

void ImmediateUserInterfaceModelViewControllerLayer::frame_start()
{
    if(std::filesystem::last_write_time(m_ViewPath) != m_ViewLastWriteTime && (m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
    {
        m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);
        m_Context->clear_cache();
    }

    if(m_Controller != nullptr)
        m_Controller->frame_start(m_Model);
}

void ImmediateUserInterfaceModelViewControllerLayer::frame_update()
{
    if(m_Context->begin_window(m_Context->next_id("File contents", "FileContents")))
    {
        if(m_ViewStatus)
        {
            m_View.get_root().traverse(
                [this](const ElementObj& _Object, const int&)
                {
                    // parse widgets
                    if(label(_Object)) return;
                    if(push_button(_Object)) return;

                    if(input_scalar<float>(_Object, "InputScalarFloat"))return;
                    if(input_scalar<int>(_Object, "InputScalarInt"))return;

                    if(input_scalar_slider<float>(_Object, "InputScalarSliderFloat"))return;
                    if(input_scalar_slider<int>(_Object, "InputScalarSliderInt"))return;

                    if(progressbar_default<float>(_Object, "ProgressBarDefaultFloat")) return;
                    if(progressbar_default<int>(_Object, "ProgressBarDefaultInt")) return;

                    if(progressbar_circular<float>(_Object, "ProgressBarCircularFloat")) return;
                    if(progressbar_circular<int>(_Object, "ProgressBarCircularInt")) return;

                    if(input_color(_Object)) return;
                    if(color_picker_rgba(_Object)) return;
                    if(color_picker_hsva(_Object)) return;

                    // parse layouts
                    if(begin_panel(_Object)) return;
                    if(begin_vertical_stack(_Object)) return;
                    if(begin_horizontal_stack(_Object)) return;
                },
                [this](const ElementObj& _Object, const int&)
                {
                    end_panel(_Object);
                    end_vertical_stack(_Object);
                    end_horizontal_stack(_Object);
                });
        }
        else
        {
            m_Context->label(
                m_Context->next_id("Error"),
                m_ViewStatus.m_Message);
        }

        m_Context->end_window();
    }
}

int ImmediateUserInterfaceModelViewControllerLayer::parse_node_settings(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(_Object.get_name() != "Settings")
        return ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults;

    ElementObj movable = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Movable";
    });

    ElementObj resizable = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Resizable";
    });

    ElementObj verticalAlignment = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "VerticalAlignment";
    });

    ElementObj horizontalAlignment = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "HorizontalAlignment";
    });

    ElementObj verticalScrollBar = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "VerticalScrollBar";
    });

    ElementObj horizontalScrollBar = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "HorizontalScrollBar";
    });

    ElementObj resizeToContentsVertically = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "ResizeToContentsVertically";
    });

    ElementObj resizeToContentsHorizontally = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "ResizeToContentsHorizontally";
    });

    int settings = 0;

    if(movable.is_not_null() && !movable.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(movable.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;
    }

    if(resizable.is_not_null() && !resizable.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(resizable.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable;
    }

    if(verticalAlignment.is_not_null() && !verticalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(verticalAlignment.get_value())) == "top")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentTop;
    }

    if(verticalAlignment.is_not_null() && !verticalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(verticalAlignment.get_value())) == "center")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentCenter;
    }

    if(verticalAlignment.is_not_null() && !verticalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(verticalAlignment.get_value())) == "bottom")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalContentAlignmentBottom;
    }

    if(horizontalAlignment.is_not_null() && !horizontalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(horizontalAlignment.get_value())) == "left")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentLeft;
    }

    if(horizontalAlignment.is_not_null() && !horizontalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(horizontalAlignment.get_value())) == "center")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentCenter;
    }

    if(horizontalAlignment.is_not_null() && !horizontalAlignment.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(horizontalAlignment.get_value())) == "right")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalContentAlignmentRight;
    }

    if(resizeToContentsVertically.is_not_null() && !resizeToContentsVertically.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(resizeToContentsVertically.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsVertically;
    }

    if(resizeToContentsHorizontally.is_not_null() && !resizeToContentsHorizontally.get_value().empty())
    {
        if(Frenchie::Core::String::utf8_to_lower(std::string(resizeToContentsHorizontally.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_ResizeToContentsHorizontally;
    }

    if(verticalScrollBar.is_not_null())
    {
        ElementObj visibility = verticalScrollBar.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Visibility";
        });

        ElementObj mouseWheelAdjustment = verticalScrollBar.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "MouseWheelAdjustment";
        });

        ElementObj arrowKeysAdjustment = verticalScrollBar.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "ArrowKeysAdjustment";
        });

        if(visibility.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "never")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverVerticalScrollBar;

        if(visibility.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "always")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysVerticalScrollBar;

        if(visibility.is_not_null() && (Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "auto" || Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "adaptive"))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveVerticalScrollBar;

        if(mouseWheelAdjustment.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(mouseWheelAdjustment.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_VerticalScrollBarMouseWheelAdjustment;

        if(arrowKeysAdjustment.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(arrowKeysAdjustment.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_VerticalScrollBarArrowKeysAdjustment;
    }

    if(horizontalScrollBar.is_not_null())
    {
        ElementObj visibility = horizontalScrollBar.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Visibility";
        });

        ElementObj arrowKeysAdjustment = horizontalScrollBar.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "ArrowKeysAdjustment";
        });

        if(visibility.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "never")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_NeverHorizontalScrollBar;

        if(visibility.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "always")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AlwaysHorizontalScrollBar;

        if(visibility.is_not_null() && (Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "auto" || Frenchie::Core::String::utf8_to_lower(std::string(visibility.get_value())) == "adaptive"))
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_AdaptiveHorizontalScrollBar;

        if(arrowKeysAdjustment.is_not_null() && Frenchie::Core::String::utf8_to_lower(std::string(arrowKeysAdjustment.get_value())) == "true")
            settings |= ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_HorizontalScrollBarArrowKeysAdjustment;
    }

    return settings <= 0 ? ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Defaults : settings;
}

#define TYPE gs_color

template<> TYPE& ImmediateUserInterfaceModelViewControllerLayer::parse_reference<TYPE>(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    ElementObj sourceObj = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Source";});

    if(!sourceObj.get_value().empty() && m_Model.find(std::string(sourceObj.get_value())) != m_Model.end())
    {
        try
        {
            return std::any_cast<std::reference_wrapper<TYPE>>(m_Model[std::string(sourceObj.get_value())]).get();
        }
        catch(...)
        {
        }
    }
    
    static TYPE defaultValue = gs_color_rgb(255, 255, 255);
    m_Model[std::string(sourceObj.get_value())] = std::reference_wrapper<TYPE>(defaultValue);
    return defaultValue;
}

#undef TYPE

bool ImmediateUserInterfaceModelViewControllerLayer::begin_panel(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Panel",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_panel(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelViewControllerLayer::end_panel(const ElementObj& _Object)
{
    if(m_Context->is_current_node_panel() && _Object.get_name() == "Panel")
        m_Context->end_panel();
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_vertical_stack(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "VerticalStack",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_vertical_stack(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelViewControllerLayer::end_vertical_stack(const ElementObj& _Object)
{
    if(m_Context->is_current_node_vertical_stack() && _Object.get_name() == "VerticalStack")
        m_Context->end_vertical_stack();
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_horizontal_stack(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "HorizontalStack",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_horizontal_stack(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelViewControllerLayer::end_horizontal_stack(const ElementObj& _Object)
{
    if(m_Context->is_current_node_horizontal_stack() && _Object.get_name() == "HorizontalStack")
        m_Context->end_horizontal_stack();
}

bool ImmediateUserInterfaceModelViewControllerLayer::begin_scrollarea(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ScrollArea",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_scrollarea(
                _ID,
                parse_node_settings(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelViewControllerLayer::end_scrollarea(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(m_Context->is_current_node_scrollarea() && _Object.get_name() == "ScrollArea")
        m_Context->end_scrollarea();
}

bool ImmediateUserInterfaceModelViewControllerLayer::label(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "Label",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            // parse text settings
            ElementObj alignmentObj = _Object.find_node([](const ElementObj& _Object)->bool
            {
                return _Object.get_name() == "Alignment";
            });

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
                parse_value<std::string>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Text";
                }),
                [](const std::string& _Value)->std::string{return _Value;}),
                
                // settings
                settings,
                
                // maximum symbols count
                parse_value<int>(
                _Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "MaximumSymbolsCount";
                }),
                [](const std::string& _Value)->int
                {
                    return !_Value.empty() ? Frenchie::Core::String::from_string<int>(_Value) : gs_huge<int>();
                }
            ));
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::push_button(const ElementObj& _Object)
{
    return parse_object(
        _Object,
        "PushButton",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            std::function<void()> callback = parse_value<std::function<void()>>(
                _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Action";}),
                [](const std::string& _Value)->std::function<void()>{return nullptr;});

            if(m_Context->push_button(_ID) && callback != nullptr)
                callback();
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::input_color(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "InputColor",
        [this](const ElementObj& _Object, const std::string& _ID)
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

            if(editRGB.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(editHSV.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(editHSL.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(editAlpha.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(colorButton.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(colorDragAndDrop.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->input_color(
                _ID,
                parse_reference<gs_color>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Value";})),
                settings);
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::color_picker_rgba(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ColorPickerRGBA",
        [this](const ElementObj& _Object, const std::string& _ID)
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

            if(editRGB.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(editHSV.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(editHSL.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(editAlpha.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(colorButton.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(colorDragAndDrop.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->color_picker_rgba(
                _ID,
                parse_reference<gs_color>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Value";})),
                settings);
        }
    );
}

bool ImmediateUserInterfaceModelViewControllerLayer::color_picker_hsva(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return parse_object(
        _Object,
        "ColorPickerHSVA",
        [this](const ElementObj& _Object, const std::string& _ID)
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

            if(editRGB.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditRGB;

            if(editHSV.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSV;

            if(editHSL.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditHSL;

            if(editAlpha.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_EditAlpha;

            if(colorButton.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorButton;

            if(colorDragAndDrop.get_value() == "true")
                settings |= ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_PreviewColorDragAndDropPane;

            if(settings == 0)
                settings = ImmediateUserInterfaceColorPickerSettings_::ImmediateUserInterfaceColorPickerSettings_Defaults;

            m_Context->color_picker_hsva(
                _ID,
                parse_reference<gs_color>(_Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Value";})),
                settings);
        }
    );
}