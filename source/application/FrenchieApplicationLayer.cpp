#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

Layer::Layer(const std::string& _Name) : m_Name(_Name){}
Layer::~Layer(){}

std::string Layer::get_name() const
{
    return m_Name;
}

bool Layer::is_closed() const
{
    return !m_Opened;
}

bool Layer::is_hidden() const
{
    return !m_Shown;
}

void Layer::close()
{
    m_Opened = false;
}

void Layer::hide()
{
    m_Shown = false;
}

bool Layer::awake()
{
    // read self state
    Frenchie::Core::Serialization::ISerializer* serializer = 
        dynamic_cast<Frenchie::Core::Serialization::ISerializer*>(this);

    if(serializer != nullptr)
    {
        auto& appState = 
            Frenchie::Application::Application::instance()->get_state();

        serializer->deserialize(appState.find_node(STRINGIFY(Application)));
    }

    return on_awake();
}

void Layer::frame_start()
{
    on_frame_start();
}

void Layer::frame_update()
{
    on_frame_update();
}

void Layer::frame_finish()
{
    on_frame_finish();
}

void Layer::finish()
{
    // save self state
    Frenchie::Core::Serialization::ISerializer* serializer = 
        dynamic_cast<Frenchie::Core::Serialization::ISerializer*>(this);

    if(serializer != nullptr)
    {
        auto& appState = 
            Frenchie::Application::Application::instance()->get_state();

        auto appInfoNode = appState.find_node(STRINGIFY(Application));

        if(!appInfoNode.is_valid()) 
            appInfoNode = appState.append_node(STRINGIFY(Application));

        serializer->serialize(appInfoNode);
    }

    // finish
    on_finish();
}

bool Layer::on_awake()
{
    return true;
}

void Layer::on_frame_start(){}
void Layer::on_frame_update(){}
void Layer::on_frame_finish(){}
void Layer::on_finish(){}

bool Layer::allows_multiple_instances() const
{
    return true;
}