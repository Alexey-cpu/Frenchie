#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

Layer::Layer(const std::string& _Name) : 
    m_Name(_Name), 
    m_UUID(Singleton<UUID4Generator>::instance()->guid()){}

Layer::~Layer(){}

std::string Layer::get_name() const
{
    return m_Name;
}

UUID4 Layer::get_uuid() const
{
    return m_UUID;
}

void Layer::set_name(const std::string& _Value)
{
    m_Name = _Value;
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

void Layer::show()
{
    m_Shown = true;
}

bool Layer::awake()
{
    // read self state
    Frenchie::Core::Serialization::ISerializer* serializer = 
        dynamic_cast<Frenchie::Core::Serialization::ISerializer*>(this);

    if(serializer != nullptr)
    {
        auto& appState = application()->get_state();
        serializer->deserialize(appState.find_node(STRINGIFY(Application)));
    }

    return true;
}

void Layer::frame_start()
{
}

void Layer::frame_update()
{
}

void Layer::frame_finish()
{
}

void Layer::finish()
{
    // save self state
    Frenchie::Core::Serialization::ISerializer* serializer = 
        dynamic_cast<Frenchie::Core::Serialization::ISerializer*>(this);

    if(serializer != nullptr)
    {
        auto& appState = 
            application()->get_state();

        auto appInfoNode = appState.find_node(STRINGIFY(Application));

        if(!appInfoNode.is_valid()) 
            appInfoNode = appState.append_node(STRINGIFY(Application));

        serializer->serialize(appInfoNode);
    }
}

bool Layer::allows_multiple_instances() const
{
    return true;
}