#include <FrenchieEditorLocalThreadQueue.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

using namespace Frenchie::Editor;

LocalThreadQueue::LocalThreadQueue(){}
LocalThreadQueue::~LocalThreadQueue(){}

Frenchie::Core::Reference<Frenchie::Application::ThreadQueue> 
LocalThreadQueue::instance()
{
    if(m_ThreadQueue == nullptr) 
        m_ThreadQueue = Frenchie::Application::application()->push_layer<Frenchie::Application::ThreadQueue>();

    return m_ThreadQueue;
}