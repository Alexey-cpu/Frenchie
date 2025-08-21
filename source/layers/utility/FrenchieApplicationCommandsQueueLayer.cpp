#include <FrenchieApplicationCommandsQueueLayer.hpp>

using namespace Frenchie::Application;

CommandsQueueLayer::CommandsQueueLayer() : Layer(STRINGIFY(CommandsQueueLayer)){}
CommandsQueueLayer::~CommandsQueueLayer(){}

void CommandsQueueLayer::frame_start()
{
    m_CommandsQueue.execute(); // execute all commands added into queue
}