#include <FrenchieApplicationEditorCommandsQueueLayer.hpp>

using namespace Frenchie::Application;

CommandsQueue::CommandsQueue() : Layer(STRINGIFY(CommandsQueue)){}
CommandsQueue::~CommandsQueue(){}

void CommandsQueue::frame_start()
{
    while(!m_Commands.empty())
    {
        auto& command = m_Commands.front();
        if(command != nullptr) 
            command->execute();
        m_Commands.pop();
    }
}