#include <FrenchieCoreObject.hpp>
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreSingleton.hpp>

#include <FrenchieRendererIRenderer.hpp>
#include <FrenchieRendererIEditor.hpp>

// IMGUI
#include <imgui.h>

// STL
#include <set>
#include <iostream>

using namespace Frenchie::Core;

// Component
Component::Component(){}

Component::~Component(){}

bool Component::is_enabled() const
{
    return m_Enabled;
}

void Component::set_enabled(bool _Value)
{
    m_Enabled = _Value;

    if(m_Enabled) 
        on_enabled();
    else 
        on_disabled();
}

void Component::on_enabled()
{
}

void Component::on_disabled()
{
}

bool Component::awake()
{
    return true;
}

void Component::frame_start(){}

void Component::frame_update(){}

void Component::frame_finish(){}

// Object
Object::Object(const std::string& _Name) : m_Name(_Name){}

Object::~Object(){}

std::list<Object*> Object::get_children() const
{
    if(m_Children.empty()) 
        return std::list<Object*>();

    std::list<Object*> children;

    for(auto&& child : m_Children) 
        children.push_back(child.get());

    return children;
}

std::string Object::get_name() const
{
    return m_Name;
}

bool Object::check_flag(int _N) const
{
    return (bool)((m_Flags >> _N) & 1);
}

void Object::set_name(const std::string& _Value)
{
    m_Name = _Value;
}

void Object::set_flag(int _N, bool _Value)
{
    if(_Value)
        m_Flags |= ((unsigned int)1 << _N);
    else 
        m_Flags &= ~((unsigned int)1 << _N);
}

bool Object::awake()
{
    for(auto&& component : m_Components)
    {
        if(component->is_enabled()) 
            component->awake();
    }

    for(auto&& child : m_Children) 
        child->awake();

    return true;
}

void Object::frame_start() 
{
    for(auto&& component : m_Components)
    {
        if(component->is_enabled()) 
            component->frame_start();
    }

    for(auto&& child : m_Children) 
        child->frame_start();
}

void Object::frame_update()
{
    for(auto&& component : m_Components)
    {
        if(component->is_enabled()) 
            component->frame_update();
    }

    for(auto&& child : m_Children) 
        child->frame_update();
}

void Object::frame_finish()
{
    for(auto&& component : m_Components)
    {
        if(component->is_enabled()) 
            component->frame_finish();
    }

    for(auto&& child : m_Children) 
        child->frame_finish();
}

void Object::draw_editor()
{
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    int id = 0;
    
    ImGui::PushID(id++);

    if (ImGui::TreeNodeEx(get_name().c_str(),
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow   | 
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed        |
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen   | 
            ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap))
    {
        // draw self editor
        draw_self();
        
        // draw components editors
        for(auto&& component : m_Components)
        {
            Frenchie::Renderer::IEditor* editor = 
                dynamic_cast<Frenchie::Renderer::IEditor*>(component.get());

            if(editor == nullptr) 
                continue; 

            ImGui::PushID(id++);
            ImGui::Checkbox("##", &component->m_Enabled);
            ImGui::SameLine();

            if (ImGui::TreeNodeEx(component->get_name().c_str(),
                ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow   | 
                ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed        |
                ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen   | 
                ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_AllowOverlap))
            {
                editor->draw_editor();
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Object::draw_self()
{
    // draw self editor
    std::string name = get_name();
    for (int i = 0; i < 512; i++) 
        Object::m_Editor.m_Name[i] = i < name.size() ? name[i] : '\0';
    
    if(ImGui::InputText("##", Object::m_Editor.m_Name, 512, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) 
        set_name(std::string(Object::m_Editor.m_Name));

}

Object::Editor Object::m_Editor = Editor();