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
ImmediateUserInterfaceModelLayer::ImmediateUserInterfaceModelLayer(
    const std::filesystem::path&                             _View,
    const std::shared_ptr<ImmediateUserInterfaceController>& _Controller) :
    Layer(STRINGIFY(ImmediateUserInterfaceModelLayer)),
    m_ViewPath(_View),
    m_Controller(_Controller){}

ImmediateUserInterfaceModelLayer::~ImmediateUserInterfaceModelLayer(){}

bool ImmediateUserInterfaceModelLayer::awake()
{
    if(m_Context == nullptr)
        m_Context = Frenchie::Application::Application::push_layer<ImmediateUserInterfaceContextLayer>();

    if((m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
        m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);

    return m_Context != nullptr;
}

void ImmediateUserInterfaceModelLayer::frame_start()
{
    if(std::filesystem::last_write_time(m_ViewPath) != m_ViewLastWriteTime)
    {
        if((m_ViewStatus = MVC::read_file(m_View, m_ViewPath)))
            m_ViewLastWriteTime = std::filesystem::last_write_time(m_ViewPath);
    }

    if(m_Controller != nullptr)
        m_Controller->set_model(m_Model);
}

static bool ready = false;

void ImmediateUserInterfaceModelLayer::frame_update()
{
    if(m_Context->begin_window(m_Context->next_id("File contents", "FileContents")))
    {
        if(m_ViewStatus)
        {
            m_View.get_root().traverse(
                [this](const ElementObj& _Object, const int&)
                {
                    if(!ready)std::cout << "b: " << _Object.get_name() << "\n";

                    // parse widgets
                    if(push_button(_Object)) return;
                    if(label(_Object)) return;

                    // parse layouts
                    if(begin_panel(_Object)) return;
                    if(begin_vertical_stack(_Object)) return;
                    if(begin_horizontal_stack(_Object)) return;
                },
                [this](const ElementObj& _Object, const int&)
                {
                    if(!ready)std::cout << "e: " << _Object.get_name() << "\n";
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

    if(!ready) ready = true;
}

void ImmediateUserInterfaceModelLayer::next_node(const ElementObj& _Object)
{
    ElementObj width = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Width";
    });

    ElementObj height = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Height";
    });

    ElementObj maximumWidth = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "MaximumWidth";
    });

    ElementObj maximumHeight = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "MaximumHeight";
    });

    ElementObj margin = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Margin";
    });

    ElementObj padding = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Padding";
    });

    if(width.is_not_null() && !width.get_value().empty())
    {
        m_Context->next_width(Frenchie::Core::String::from_string<float>(std::string(width.get_value())));
    }

    if(height.is_not_null() && !height.get_value().empty())
        m_Context->next_height(Frenchie::Core::String::from_string<float>(std::string(height.get_value())));

    if(maximumWidth.is_not_null() && !maximumWidth.get_value().empty())
        m_Context->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumWidth.get_value())));

    if(maximumHeight.is_not_null() && !maximumHeight.get_value().empty())
        m_Context->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumHeight.get_value())));

    if(margin.is_not_null())
    {
        ElementObj top = margin.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Top";
        });

        ElementObj left = margin.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Left";
        });

        ElementObj right = margin.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Right";
        });

        ElementObj bottom = margin.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Bottom";
        });

        m_Context->next_content_margin(
            gs_vec4f(
                !top.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                !left.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                !right.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
    }

    if(padding.is_not_null())
    {
        ElementObj top = padding.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Top";
        });

        ElementObj left = padding.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Left";
        });

        ElementObj right = padding.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Right";
        });

        ElementObj bottom = padding.find_node([](const ElementObj& _Object)->bool
        {
            return _Object.get_name() == "Bottom";
        });

        m_Context->next_content_padding(
            gs_vec4f(
                !top.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                !left.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                !right.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
    }
}

int ImmediateUserInterfaceModelLayer::layout_hints(const Frenchie::Core::Serizliation::ElementObj& _Object)
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

bool ImmediateUserInterfaceModelLayer::begin_panel(const ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "Panel",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_panel(
                _ID,
                layout_hints(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelLayer::end_panel(const ElementObj& _Object)
{
    if(m_Context->is_current_node_panel() && _Object.get_name() == "Panel")
        m_Context->end_panel();
}

bool ImmediateUserInterfaceModelLayer::begin_vertical_stack(const ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "VerticalStack",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_vertical_stack(
                _ID,
                layout_hints(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelLayer::end_vertical_stack(const ElementObj& _Object)
{
    if(m_Context->is_current_node_vertical_stack() && _Object.get_name() == "VerticalStack")
        m_Context->end_vertical_stack();
}

bool ImmediateUserInterfaceModelLayer::begin_horizontal_stack(const ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "HorizontalStack",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_horizontal_stack(
                _ID,
                layout_hints(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelLayer::end_horizontal_stack(const ElementObj& _Object)
{
    if(m_Context->is_current_node_horizontal_stack() && _Object.get_name() == "HorizontalStack")
        m_Context->end_horizontal_stack();
}

bool ImmediateUserInterfaceModelLayer::begin_scrollarea(const ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "ScrollArea",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            m_Context->begin_scrollarea(
                _ID,
                layout_hints(_Object.find_node([](const ElementObj& _Object)->bool
                {
                    return _Object.get_name() == "Settings";
                })));
        }
    );
}

void ImmediateUserInterfaceModelLayer::end_scrollarea(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(m_Context->is_current_node_scrollarea() && _Object.get_name() == "ScrollArea")
        m_Context->end_scrollarea();
}

bool ImmediateUserInterfaceModelLayer::push_button(const ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "PushButton",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            ElementObj actionObj = _Object.find_node([](const ElementObj& _Object)->bool
            {
                return _Object.get_name() == "Action";
            });

            std::function<void()> callback = parse_value<std::function<void()>>(
                actionObj,
                [](const std::string& _Value)->std::function<void()>
                {
                    return nullptr;
                });

            if(m_Context->push_button(_ID) && callback != nullptr)
                callback();
        }
    );
}

bool ImmediateUserInterfaceModelLayer::label(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    return try_parse_object(
        _Object,
        "Label",
        [this](const ElementObj& _Object, const std::string& _ID)
        {
            // parse text settings
            ElementObj settingsObj = _Object.find_node([](const ElementObj& _Object)->bool
            {
                return _Object.get_name() == "Text";
            });

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