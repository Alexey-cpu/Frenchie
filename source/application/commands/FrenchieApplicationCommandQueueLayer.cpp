#include <FrenchieApplicationCommandQueueLayer.hpp>

using namespace Frenchie::Application;

CommandQueue::CommandQueue() : Layer(STRINGIFY(CommandQueue)){}
CommandQueue::~CommandQueue(){}

void CommandQueue::frame_start()
{
    while(!m_Commands.empty())
    {
        auto& command = m_Commands.front();
        if(command != nullptr) 
            command->execute();
        m_Commands.pop();
    }
}

bool CommandQueue::allows_multiple_instances() const
{
    return false;
}