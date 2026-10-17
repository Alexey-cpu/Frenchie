#include <FrenchieEngineWorld.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Engine;

World::World() : Layer("World"){}
World::~World(){}

bool World::awake()
{
    return true;
}

void World::frame_start()
{
    for(auto it = m_Awakes.begin(); it != m_Awakes.end(); it++)
    {
        if((*it)->awake(*m_Scene))
            m_Systems.push_back((*it));
    }
    
    m_Awakes.clear();
    for(auto layer : m_Systems)
        layer->frame_start(*m_Scene);
}

void World::frame_update()
{
    for(auto& system : m_Systems)
        system->frame_update(*m_Scene);
}

void World::frame_input()
{
    for(auto& system : m_Systems)
        system->frame_input(*m_Scene);
}

void World::frame_render()
{
    for(auto& system : m_Systems)
        system->frame_render(*m_Scene);
}

void World::frame_finish()
{
    for(auto& system : m_Systems)
        system->frame_finish(*m_Scene);
}

void World::finish()
{
    for(auto& system : m_Systems)
        system->finish(*m_Scene);
}

void World::quit()
{
    for(auto& system : m_Systems)
        system->quit(*m_Scene);
}

bool World::allows_multiple_instances() const
{
    return false;
}