#include <FrenchieApplicationLayer.hpp>

using namespace Frenchie::Application;

Layer::Layer(const std::string& _Name) : m_Name(_Name){}
Layer::~Layer(){}

std::string Layer::get_name() const
{
    return m_Name;
}

bool Layer::is_closed()
{
    return !m_Opened;
}

void Layer::close()
{
    m_Opened = false;
}

bool Layer::awake()
{
    return true;
}

void Layer::frame_start(){}
void Layer::frame_update(){}
void Layer::frame_finish(){}
void Layer::finish(){}

bool Layer::allows_multiple_instances() const
{
    return true;
}