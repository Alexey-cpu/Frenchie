#include <FrenchieApplicationRendererLayer.hpp>

#include <FrenchieApplication.hpp>

using namespace Frenchie::Application;

Renderer::Renderer() : Layer(STRINGIFY(Renderer)){}
Renderer::~Renderer(){}

bool Renderer::awake()
{
    return true;
}

void Renderer::frame_start()
{
}

void Renderer::frame_update()
{
}

void Renderer::frame_render()
{
}

void Renderer::frame_finish()
{
}

void Renderer::finish()
{
}

void Renderer::quit()
{
}

bool Renderer::allows_multiple_instances() const
{
    return false;
}