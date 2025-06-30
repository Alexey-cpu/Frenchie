#include <FrenchieRendererTransformComponent.hpp>

#include <imgui.h>

#include <iostream>

using namespace Frenchie::Renderer;

Transform::Transform() : Component::Registry<Transform>(STRINGIFY(Transform)){}

Transform::~Transform(){}

glm::vec3 Transform::get_position() const
{
    return m_Position;
}

glm::vec3 Transform::get_rotation() const
{
    return m_Rotation;
}

glm::vec3 Transform::get_scale() const
{
    return m_Scale;
}

glm::mat4 Transform::get_model_matrix() const
{
    return m_ModelMatrix;
}

void Transform::set_position(const glm::vec3& _Value)
{
    m_Position = _Value;
}

void Transform::set_rotation(const glm::vec3& _Value)
{
    m_Rotation = _Value;
}

void Transform::set_scale(const glm::vec3& _Value)
{
    m_Scale = _Value;
}

bool Transform::awake()
{
    return Component::awake();
}

void Transform::frame_start()
{
    Component::frame_start();
}

void Transform::frame_update()
{
    auto object = get_object();

    if(object == nullptr) 
        return;

    // compute geometry
    auto parent = 
        object->get_parent() != nullptr ? 
            object->get_parent()->get_component<Transform>() : 
                nullptr;

    m_ModelMatrix = 
        parent != nullptr ? 
            parent->get_model_matrix() * compute_local_model_matrix() : 
                compute_local_model_matrix();
}

void Transform::frame_finish()
{
    Component::frame_finish();
}

void Transform::draw()
{
    auto transformPosition = get_position();
    auto transformRotation = get_rotation();
    auto transformScale    = get_scale();

    for (int i = 0; i < 3; i++)
    {
        // read position
        auto position = std::to_string(transformPosition[i]);
        auto rotation = std::to_string(transformRotation[i]);
        auto scale = std::to_string(transformScale[i]);

        for(int j = 0; j < 64; j++)
        {
            m_Editor.m_Position[i][j] = j < position.size() ? position[j] : '\0';
            m_Editor.m_Rotation[i][j] = j < rotation.size() ? rotation[j] : '\0';
            m_Editor.m_Scale   [i][j] = j < scale.size() ? scale[j] : '\0';
        }
    }

    std::vector<std::string> axis    = {"X", "Y", "Z"};
    std::vector<std::string> labels  = {"##", "###", "####"};

    int j = 0;

    if(ImGui::BeginTable(
        "Transform", 
        3, 
        //ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
        ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
        ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
        ImGuiTableFlags_::ImGuiTableFlags_BordersV     | 
        ImGuiTableFlags_::ImGuiTableFlags_Resizable    | 
        ImGuiTableFlags_::ImGuiTableFlags_Reorderable  | 
        ImGuiTableFlags_::ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Position",ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Rotation", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        ImGui::TableNextRow();

        for (int i = 0; i < 3; i++)
        {
            auto avail = ImGui::GetContentRegionAvail();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // position
            ImGui::PushID(j++);
            ImGui::Text(axis[i].c_str());
            ImGui::SameLine();
            ImGui::SetNextItemWidth(avail.x);
            ImGui::InputText(
                labels[i].c_str(), 
                m_Editor.m_Position[i], 
                64, 
                ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopID();

            // rotation
            ImGui::TableSetColumnIndex(1);
            ImGui::PushID(j++);
            ImGui::Text(axis[i].c_str());
            ImGui::SameLine();
            ImGui::SetNextItemWidth(avail.x);
            ImGui::InputText(
                labels[i].c_str(), 
                m_Editor.m_Rotation[i], 
                64, 
                ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopID();

            // scale
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(j++);
            ImGui::Text(axis[i].c_str());
            ImGui::SameLine();
            ImGui::SetNextItemWidth(avail.x);
            ImGui::InputText(
                labels[i].c_str(), 
                m_Editor.m_Scale[i], 
                64, 
                ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    // setup new parameters
    {    
        auto x = std::string(m_Editor.m_Position[0]);
        auto y = std::string(m_Editor.m_Position[1]);
        auto z = std::string(m_Editor.m_Position[2]);

        set_position(glm::vec3(
            x.empty() ? 0.f : std::stod(x),
            y.empty() ? 0.f : std::stod(y),
            z.empty() ? 0.f : std::stod(z)));
    }

    {
        auto x = std::string(m_Editor.m_Scale[0]);
        auto y = std::string(m_Editor.m_Scale[1]);
        auto z = std::string(m_Editor.m_Scale[2]);
        
        set_scale(glm::vec3(
            x.empty() ? 0.f : std::stod(x),
            y.empty() ? 0.f : std::stod(y),
            z.empty() ? 0.f : std::stod(z)));
    }

    { 
        auto x = std::string(m_Editor.m_Rotation[0]);
        auto y = std::string(m_Editor.m_Rotation[1]);
        auto z = std::string(m_Editor.m_Rotation[2]);

        set_rotation(glm::vec3(
            x.empty() ? 0.f : std::stod(x),
            y.empty() ? 0.f : std::stod(y),
            z.empty() ? 0.f : std::stod(z)));
    }
}

glm::mat4 Transform::compute_local_model_matrix() const
{
    glm::mat4 matrix(1.f);

    return glm::translate(matrix, m_Position) * 
            glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
            glm::rotate(matrix, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f)) * 
            glm::rotate(matrix, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f)) * 
            glm::scale(matrix, m_Scale);
}

Transform::Editor Transform::m_Editor = Editor();