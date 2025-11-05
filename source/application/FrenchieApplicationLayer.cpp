#include <FrenchieApplicationLayer.hpp>
#include <FrenchieCoreISerializer.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieApplication.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Application;

Layer::Layer(const std::string& _Name) : 
    m_Name(_Name){}

Layer::~Layer(){}

std::string Layer::get_name() const
{
    return m_Name;
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
    return true;
}

void Layer::frame_start()
{
}

void Layer::frame_update()
{
}

void Layer::frame_render()
{
}

void Layer::frame_finish()
{
}

void Layer::finish()
{
}

void Layer::quit()
{
}

bool Layer::allows_multiple_instances() const
{
    return true;
}