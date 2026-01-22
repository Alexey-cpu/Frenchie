#include <ImmediateUserInterfaceTestLayer.hpp>

using namespace Frenchie::Application;

ImmediateUserInterfaceTestLayer::ImmediateUserInterfaceTestLayer(){}
ImmediateUserInterfaceTestLayer::~ImmediateUserInterfaceTestLayer(){}

bool ImmediateUserInterfaceTestLayer::awake()
{
    if(m_ImmediateUserInterface == nullptr)
        m_ImmediateUserInterface = Frenchie::Application::application()->push_layer<Frenchie::Application::ImmediateUserInterfaceContextLayer>();

    return true;
}

void ImmediateUserInterfaceTestLayer::frame_update()
{
    auto create_horizontal_layout = [this](const std::string& _Name)
    {
        ImmediateUserInterfaceNodeSettings settins =
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;
        
        if(m_ImmediateUserInterface->begin_horizontal_stack(_Name, settins))
        {
            if(m_ImmediateUserInterface->begin_vertial_stack(std::string(_Name).append("/Child-1"), settins))
            {
                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-2"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-3"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                m_ImmediateUserInterface->end_vertical_stack();
            }

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-2"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-3"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-4"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            m_ImmediateUserInterface->end_horizontal_stack();
        }
    };

    auto create_vertical_layout = [this](const std::string& _Name)
    {
        ImmediateUserInterfaceNodeSettings settins =
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
            ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable;
        
        if(m_ImmediateUserInterface->begin_vertial_stack(_Name, settins))
        {
            if(m_ImmediateUserInterface->begin_vertial_stack(std::string(_Name).append("/Child-1"), settins))
            {
                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-1"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-2"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-1/Child-3"), settins))
                    m_ImmediateUserInterface->end_horizontal_stack();

                m_ImmediateUserInterface->end_vertical_stack();
            }

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-2"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-3"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            if(m_ImmediateUserInterface->begin_horizontal_stack(std::string(_Name).append("/Child-4"), settins))
                m_ImmediateUserInterface->end_horizontal_stack();

            m_ImmediateUserInterface->end_vertical_stack();
        }
    };

    create_horizontal_layout("Layout-1");

    // if(m_ImmediateUserInterface->begin_window(
    //     "Window-1",
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
    //     ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    // {
    //     create_horizontal_layout("Window-1/Layout");
    //     m_ImmediateUserInterface->end_window();
    // }

    if(m_ImmediateUserInterface->begin_window(
        "Window-2",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        create_horizontal_layout("Window-2/Layout");
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-3",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {
        create_horizontal_layout("Window-3/Layout");
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-4",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {        
        m_ImmediateUserInterface->end_window();
    }

    if(m_ImmediateUserInterface->begin_window(
        "Window-5",
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Resizable |
        ImmediateUserInterfaceNodeSettings_::ImmediateUserInterfaceNodeSettings_Movable))
    {        
        m_ImmediateUserInterface->end_window();
    }
}

void ImmediateUserInterfaceTestLayer::finish()
{
    if(m_ImmediateUserInterface != nullptr)
        m_ImmediateUserInterface->close();
}