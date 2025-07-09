#include <FrenchieRendererSizeComponent.hpp>

using namespace Frenchie::Renderer;

// IMGUI
#include <imgui.h>

Size::Size(const glm::vec2& _Size) : Frenchie::Core::Component::Registry<Size>(STRINGIFY(Size)){}
Size::~Size(){}

// getters
glm::vec2 Size::get_size() const
{
    return m_Size;
}

void Size::set_size(const glm::vec2& _Value)
{
    m_Size = _Value;
}

// IRenderer
bool Size::awake()
{
    return true;
}

void Size::frame_start()
{
}

void Size::frame_update()
{
}

void Size::frame_finish()
{
}

void Size::draw()
{
    ImGui::DragFloat2("min ", &m_Size[0], 0.1f, 0.f, +INFINITY, "%.4f");
}