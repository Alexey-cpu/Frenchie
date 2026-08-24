#include <FrenchieImmediateUserInterfaceModelLayer.hpp>

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
            Frenchie::Core::Serizliation::DOMTree::Status read_file(const Frenchie::Core::Serizliation::DOMTree& _Document, const std::filesystem::path& _File)
            {
                if(!std::filesystem::exists(_File) || std::filesystem::is_directory(_File))
                {
                    return Frenchie::Core::Serizliation::DOMTree::Status(
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
                        return Frenchie::Core::Serizliation::DOMTree::Status(false, std::string("unknown file format ").append(extention));
            }
        }
    }
}

// ImmediateUserInterfaceModelLayer
ImmediateUserInterfaceModelLayer::ImmediateUserInterfaceModelLayer(const std::filesystem::path& _Path) :
    Layer(STRINGIFY(ImmediateUserInterfaceModelLayer)), m_ViewPath(_Path){}

ImmediateUserInterfaceModelLayer::~ImmediateUserInterfaceModelLayer(){}

bool ImmediateUserInterfaceModelLayer::awake()
{
    if(m_Controller == nullptr)
        m_Controller = Frenchie::Application::Application::push_layer<ImmediateUserInterfaceContextLayer>();

    read_file();

    return m_Controller != nullptr;
}

void ImmediateUserInterfaceModelLayer::frame_start()
{
    if(!check_status()) return;

    if(std::filesystem::last_write_time(m_ViewPath) == m_ViewLastWriteTime)
        return;

    read_file();
}

static bool ready = false;

void ImmediateUserInterfaceModelLayer::frame_update()
{
    if(m_Controller->begin_window(m_Controller->next_id("File contents", "FileContents")))
    {
        if(m_ViewStatus)
        {
            m_View.get_root().traverse(
                [this](const ElementObj& _Object, const int&)
                {
                    if(!ready)std::cout << "b: " << _Object.get_name() << "\n";

                    // parse widgets
                    if(push_button(_Object)) return;

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
            m_Controller->label(m_Controller->next_id("Error"), m_ViewStatus.m_Message);
        }

        m_Controller->end_window();
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
        m_Controller->next_width(Frenchie::Core::String::from_string<float>(std::string(width.get_value())));

        if(!ready) std::cout << "parsing width " << width.get_value() << "\n";
    }

    if(height.is_not_null() && !height.get_value().empty())
        m_Controller->next_height(Frenchie::Core::String::from_string<float>(std::string(height.get_value())));

    if(maximumWidth.is_not_null() && !maximumWidth.get_value().empty())
        m_Controller->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumWidth.get_value())));

    if(maximumHeight.is_not_null() && !maximumHeight.get_value().empty())
        m_Controller->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumHeight.get_value())));

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

        m_Controller->next_content_margin(
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

        m_Controller->next_content_padding(
            gs_vec4f(
                !top.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                !left.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                !right.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
    }
}

int ImmediateUserInterfaceModelLayer::layout_hints(const Frenchie::Core::Serizliation::ElementObj& _Object)
{
    if(_Object.get_name() != "LayoutHints")
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

    int settings = -1;

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
    if(_Object.get_name() != "Panel")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_Controller->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_Controller->begin_panel(m_Controller->next_id(std::string(name.get_value()), std::string(hash.get_value())), layout_hints(_Object));

    return true;
}

void ImmediateUserInterfaceModelLayer::end_panel(const ElementObj& _Object)
{
    if(m_Controller->is_current_node_panel() && _Object.get_name() == "Panel")
        m_Controller->end_panel();
}

bool ImmediateUserInterfaceModelLayer::begin_vertical_stack(const ElementObj& _Object)
{
    if(_Object.get_name() != "VerticalStack")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_Controller->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_Controller->begin_vertical_stack(m_Controller->next_id(std::string(name.get_value()), std::string(hash.get_value())), layout_hints(_Object));

    return true;
}

void ImmediateUserInterfaceModelLayer::end_vertical_stack(const ElementObj& _Object)
{
    if(m_Controller->is_current_node_vertical_stack() && _Object.get_name() == "VerticalStack")
        m_Controller->end_vertical_stack();
}

bool ImmediateUserInterfaceModelLayer::begin_horizontal_stack(const ElementObj& _Object)
{
    if(_Object.get_name() != "HorizontalStack")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_Controller->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_Controller->begin_horizontal_stack(m_Controller->next_id(std::string(name.get_value()), std::string(hash.get_value())), layout_hints(_Object));
        
    return true;
}

void ImmediateUserInterfaceModelLayer::end_horizontal_stack(const ElementObj& _Object)
{
    if(m_Controller->is_current_node_horizontal_stack() && _Object.get_name() == "HorizontalStack")
        m_Controller->end_horizontal_stack();
}

bool ImmediateUserInterfaceModelLayer::push_button(const ElementObj& _Object)
{
    if(_Object.get_name() != "PushButton")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Name";
    });

    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool
    {
        return _Object.get_name() == "Hash";
    });

    if(name.get_value().empty() || m_Controller->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_Controller->push_button(
        m_Controller->next_id(
            std::string(name.get_value()),
            std::string(hash.get_value())));

    return true;
}