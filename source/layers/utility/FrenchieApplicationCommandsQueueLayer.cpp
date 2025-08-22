#include <FrenchieApplicationCommandsQueueLayer.hpp>

using namespace Frenchie::Application;

CommandsQueue::CommandsQueue() : Layer(STRINGIFY(CommandsQueue)){}
CommandsQueue::~CommandsQueue(){}

void CommandsQueue::frame_start()
{
    m_CommandsQueue.execute(); // execute all commands added into queue
}