#include <FrenchieApplicationLayer.hpp>

using namespace Frenchie::Application;

Layer::Layer(const std::string& _Name) : m_Name(_Name)
{
}

Layer::~Layer()
{
}

std::string Layer::get_name() const
{
    return m_Name;
}

void Layer::awake()
{
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
}

void Layer::close()
{
    m_Closed = true;
}

bool Layer::is_closed()
{
    return m_Closed;
}