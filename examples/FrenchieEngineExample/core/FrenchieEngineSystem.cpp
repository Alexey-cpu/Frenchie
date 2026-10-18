#include <FrenchieEngineSystem.hpp>

using namespace Frenchie::Engine;

System::System(){}
System::~System(){}

bool System::awake(Scene&)
{
    return true;
}

void System::frame_start(Scene&){}

void System::frame_update(Scene&){}

void System::frame_input(Scene&){}

void System::frame_render(Scene&){}

void System::frame_finish(Scene&){}

void System::finish(Scene&){}

void System::quit(Scene&){}

bool System::is_running() const
{
    return m_IsRunning;
}

void System::shut_down()
{
    m_IsRunning = false;
}