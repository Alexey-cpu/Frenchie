#include <FrenchieRendererSizeComponent.hpp>

using namespace Frenchie::Renderer;

// IMGUI
#include <imgui.h>

Size::Size(const glm::vec2& _Size) : Frenchie::Core::Component::Registry<Size>(STRINGIFY(Size)), m_Size(_Size){}
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

float Size::get_aspect() const
{
    return m_Size.x / m_Size.y;
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

void Size::draw_editor()
{
    ImGui::DragFloat2("min ", &m_Size[0], 0.1f, 0.f, +INFINITY, "%.4f");
}