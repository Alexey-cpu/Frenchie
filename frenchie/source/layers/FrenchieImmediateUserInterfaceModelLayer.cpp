#include <FrenchieImmediateUserInterfaceModelLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::String;
using namespace Frenchie::Core::Serizliation;
using namespace Frenchie::Application;

ImmediateUserInterfaceModelLayer::ImmediateUserInterfaceModelLayer(const std::filesystem::path& _Path) :
    Layer(STRINGIFY(ImmediateUserInterfaceModelLayer)), m_ViewFilePath(_Path){}

ImmediateUserInterfaceModelLayer::~ImmediateUserInterfaceModelLayer(){}

bool ImmediateUserInterfaceModelLayer::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::Application::push_layer<ImmediateUserInterfaceContextLayer>();

    if(std::filesystem::exists(m_ViewFilePath))
    {
        m_ViewFileLastWriteTime = std::filesystem::last_write_time(m_ViewFilePath);
        m_ViewFileParseStatus   = m_View.read_file<Frenchie::Core::Serizliation::XML::Parser>(Frenchie::Core::String::convert_utf32_to_utf8(m_ViewFilePath.u32string()));
    }

    return m_UI != nullptr;
}

void ImmediateUserInterfaceModelLayer::frame_start()
{
    if(!std::filesystem::exists(m_ViewFilePath) || std::filesystem::last_write_time(m_ViewFilePath) == m_ViewFileLastWriteTime)
        return;

    m_ViewFileParseStatus = 
        m_View.read_file<Frenchie::Core::Serizliation::XML::Parser>(Frenchie::Core::String::convert_utf32_to_utf8(m_ViewFilePath.u32string()));
}

static bool ready = false;

void ImmediateUserInterfaceModelLayer::frame_update()
{
    if(m_UI->begin_window(m_UI->next_id("File contents", "FileContents")))
    {
        if(m_ViewFileParseStatus)
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
            m_UI->label(m_UI->next_id("Error"), m_ViewFileParseStatus.m_Message);
        }

        m_UI->end_window();
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
        m_UI->next_width(Frenchie::Core::String::from_string<float>(std::string(width.get_value())));

        if(!ready) std::cout << "parsing width " << width.get_value() << "\n";
    }

    if(height.is_not_null() && !height.get_value().empty())
        m_UI->next_height(Frenchie::Core::String::from_string<float>(std::string(height.get_value())));

    if(maximumWidth.is_not_null() && !maximumWidth.get_value().empty())
        m_UI->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumWidth.get_value())));

    if(maximumHeight.is_not_null() && !maximumHeight.get_value().empty())
        m_UI->next_maximum_width(Frenchie::Core::String::from_string<float>(std::string(maximumHeight.get_value())));

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

        m_UI->next_content_margin(
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

        m_UI->next_content_padding(
            gs_vec4f(
                !top.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(top.get_value())) : 0.f,
                !left.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(left.get_value())) : 0.f,
                !right.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(right.get_value())) : 0.f,
                !bottom.get_value().empty() ? Frenchie::Core::String::from_string<float>(std::string(bottom.get_value())) : 0.f));
    }
}

bool ImmediateUserInterfaceModelLayer::begin_panel(const ElementObj& _Object)
{
    if(_Object.get_name() != "Panel")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_UI->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_UI->begin_panel(
        m_UI->next_id(
            std::string(name.get_value()),
            std::string(hash.get_value())));

    return true;
}

void ImmediateUserInterfaceModelLayer::end_panel(const ElementObj& _Object)
{
    if(m_UI->is_current_node_panel() && _Object.get_name() == "Panel")
        m_UI->end_panel();
}

bool ImmediateUserInterfaceModelLayer::begin_vertical_stack(const ElementObj& _Object)
{
    if(_Object.get_name() != "VerticalStack")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_UI->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_UI->begin_vertical_stack(
        m_UI->next_id(
            std::string(name.get_value()),
            std::string(hash.get_value())));

    return true;
}

void ImmediateUserInterfaceModelLayer::end_vertical_stack(const ElementObj& _Object)
{
    if(m_UI->is_current_node_vertical_stack() && _Object.get_name() == "VerticalStack")
        m_UI->end_vertical_stack();
}

bool ImmediateUserInterfaceModelLayer::begin_horizontal_stack(const ElementObj& _Object)
{
    if(_Object.get_name() != "HorizontalStack")
        return false;

    ElementObj name = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Name";});
    ElementObj hash = _Object.find_node([](const ElementObj& _Object)->bool{return _Object.get_name() == "Hash";});
    if(name.get_value().empty() || m_UI->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_UI->begin_horizontal_stack(
        m_UI->next_id(
            std::string(name.get_value()),
            std::string(hash.get_value())));
        
    return true;
}

void ImmediateUserInterfaceModelLayer::end_horizontal_stack(const ElementObj& _Object)
{
    if(m_UI->is_current_node_horizontal_stack() && _Object.get_name() == "HorizontalStack")
        m_UI->end_horizontal_stack();
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

    if(name.get_value().empty() || m_UI->does_node_exist(std::string(name.get_value()), std::string(hash.get_value())))
        return false;

    next_node(_Object);

    m_UI->push_button(
        m_UI->next_id(
            std::string(name.get_value()),
            std::string(hash.get_value())));

    return true;
}