#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

Application::Application(){}
Application::~Application(){}

std::string Application::get_name() const
{
    return m_Name;
}

void Application::set_name(const std::string& _Name)
{
    m_Name = _Name;
}

bool Application::is_closed()
{
    return m_Closed;
}

void Application::close()
{
    m_Closed = true;
}

int Application::execute()
{
    if(!awake()) 
        return -1;

    while (!is_closed())
    {
        // remove layers that are closed
        for(auto it = m_Layers.begin(); it != m_Layers.end(); it++)
        {
            if((*it)->is_closed())
            {
                (*it)->finish();
                auto rm = it;
                it++;
                m_Layers.erase(rm);

                if(it == m_Layers.end())
                    break;
            }
        }

        frame_start();
        frame_update();
        frame_finish();
    }

    finish();

    return 1;
}