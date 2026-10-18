#pragma once

#include <FrenchieEngineEditor.hpp>

using namespace Frenchie::Application;
using namespace Frenchie::Engine;

Editor::Editor() : Layer(STRINGIFY(Editor)){}
Editor::~Editor(){}

bool Editor::awake()
{
    if(m_UI == nullptr)
        m_UI = Frenchie::Application::App::push_layer<ImmediateUserInterfaceContextLayer>();

    return m_UI != nullptr;
}

void Editor::frame_start()
{
}

void Editor::frame_update()
{
    if(m_UI->begin_menubar(m_UI->next_id("FrenchieEngineMenuBar", "FrenchieEngineMenuBar")))
    {
        m_UI->end_menubar();
    }
}

void Editor::frame_input(){}

void Editor::frame_render(){}

void Editor::frame_finish(){}

void Editor::finish(){}

void Editor::quit(){}