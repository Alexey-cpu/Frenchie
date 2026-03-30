#include <FrenchieEditorTerminalLayer.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Editor;

Terminal::Terminal(){}
Terminal::~Terminal(){}

bool Terminal::awake()
{
    TerminalLogger::instance()->set_level(spdlog::level::level_enum::trace);

    return true;
}

bool Terminal::allows_multiple_instances() const
{
    return false;
}

Frenchie::Core::Reference<Terminal> Terminal::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Terminal>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Terminal>();

    return layer;
}