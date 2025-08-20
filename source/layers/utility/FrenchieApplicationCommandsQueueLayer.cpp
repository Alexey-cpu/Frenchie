#include <FrenchieApplicationCommandsQueueLayer.hpp>

using namespace Frenchie::Application;

CommandsQueueLayer::CommandsQueueLayer() : Layer(STRINGIFY(CommandsQueueLayer)){}
CommandsQueueLayer::~CommandsQueueLayer(){}

void CommandsQueueLayer::on_frame_start()
{
    m_CommandsQueue.execute(); // execute all commands added into queue
}