#include <FrenchieRendererScene3DCamera.hpp>

#include <FrenchieRendererScene3D.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Renderer;

Camera::Camera(
    glm::vec3 _CameraWorldPosition, 
    glm::vec3 _CameraWorldUpAxisDirection) :
    Component::Registry<Camera>(STRINGIFY(Camera)),
    m_CameraWorldPosition(_CameraWorldPosition), 
    m_CameraWorldUpAxisDirection(_CameraWorldUpAxisDirection)
{

}

Camera::~Camera(){}

glm::mat4 Camera::get_view_matrix() const
{
    // camera rotation angles
    glm::mat4 rotateX  = glm::rotate(glm::mat4(1.f), glm::radians(m_Pitch), glm::vec3(1.f, 0.f, 0.f));
    glm::mat4 rotateY  = glm::rotate(glm::mat4(1.f), glm::radians(m_Yaw), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 rotateZ  = glm::rotate(glm::mat4(1.f), glm::radians(m_Roll), glm::vec3(0.f, 0.f, 1.f));

    // camera local attributes
    m_CameraLocalFrontAxisDirection = glm::normalize(rotateY * rotateX * glm::vec4(glm::vec3(0.f, 0.f, -m_Axis.z), 1.f));
    m_CameraLocalRightAxisDirection = glm::normalize(glm::cross(m_CameraLocalFrontAxisDirection, m_CameraWorldUpAxisDirection));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::cross(m_CameraLocalRightAxisDirection, m_CameraLocalFrontAxisDirection));
    
    // rotate around Z axis
    m_CameraLocalFrontAxisDirection = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalFrontAxisDirection, 1.f)));
    m_CameraLocalUpAxisDirection    = glm::normalize(glm::vec3(rotateZ * glm::vec4(m_CameraLocalUpAxisDirection, 1.f)));

    glm::vec3 cameraPosition = get_scale_matrix() * glm::vec4(m_CameraWorldPosition, 1.f);

    return glm::lookAt(
        cameraPosition, 
        cameraPosition + m_CameraLocalFrontAxisDirection, 
        m_CameraLocalUpAxisDirection);
}

glm::mat4 Camera::get_scale_matrix() const
{
    return glm::scale(
        glm::mat4(1.f), 
        glm::vec3(
            1.f / std::max<float>(m_Resolution.x, 1.f), 
            1.f / std::max<float>(m_Resolution.y, 1.f), 
            1.f / 10000.f
        )
    );
}

glm::mat4 Camera::get_projection_matrix() const
{
    return glm::perspective(glm::radians(get_fovy()), 1.f, get_near(), get_far()) * get_view_matrix() * get_scale_matrix();
}

glm::vec3 Camera::get_position() const
{
    return m_CameraWorldPosition;
}

glm::vec2 Camera::get_resolution() const
{
    return m_Resolution;
}

glm::vec3 Camera::get_axis() const
{
    return m_Axis;
}

glm::vec3 Camera::get_front() const
{
    return m_CameraLocalFrontAxisDirection;
}

glm::vec3 Camera::get_up() const
{
    return m_CameraLocalUpAxisDirection;
}

glm::vec3 Camera::get_right() const
{
    return m_CameraLocalRightAxisDirection;
}

float Camera::get_aspect() const
{
    return m_Resolution.x / m_Resolution.y;
}

float Camera::get_near() const
{
    return m_Near;
}

float Camera::get_far() const
{
    return m_Far;
}

float Camera::get_fovy() const
{
    return m_Fovy;
}

void Camera::set_axis(const glm::vec3& _Value)
{
    m_Axis = _Value;
}

float Camera::get_movement_speed() const
{
    return m_MovementSpeed;
}

float Camera::get_sensitivity() const
{
    return m_Sensitivity;
}

float Camera::get_pitch() const
{
    return m_Pitch;
}

float Camera::get_yaw() const
{
    return m_Yaw;
}

float Camera::get_roll() const
{
    return m_Roll;
}

unsigned int Camera::get_texture() const
{
    return m_TextureColorBuffer;
}

void Camera::set_position(const glm::vec3& _Value)
{
    m_CameraWorldPosition = _Value;
}

void Camera::set_resolution(const glm::vec2& _Resolution)
{
    m_Resolution = _Resolution;
}

void Camera::set_movement_speed(const float& _Value)
{
    m_MovementSpeed = _Value;
}

void Camera::set_sensitivity(const float& _Value)
{
    m_Sensitivity = _Value;
}

void Camera::set_pitch(const float& _Value)
{
    if(_Value > 89.0f) 
        m_Pitch = 89.0f;
    else if(_Value < -89.0f) 
        m_Pitch = -89.0f;
    else 
        m_Pitch = _Value;
}

void Camera::set_yaw(const float& _Value)
{
    m_Yaw = _Value;
}

void Camera::set_roll(const float& _Value)
{
    m_Roll = _Value;
}

void Camera::set_near(const float& _Value)
{
    m_Near = _Value;
}

void Camera::set_far(const float& _Value)
{
    m_Far = _Value;
}

void Camera::set_fovy(const float& _Value)
{
    m_Fovy = _Value;
}

bool Camera::awake()
{
    int width  = 2048;
    int height = 1024;

    // create frame buffer
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    
    // create a color attachment texture
    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &m_TextureDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer); // now actually attach it
    
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::instance()->error("FRENCHIE::APPLICATION::OBJECT_SCREEN_SHOTER::AWAKE_FAILED");
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void Camera::frame_start(){}

void Camera::frame_update(){}

void Camera::frame_finish(){}

void Camera::draw_editor() 
{
    // camera geometry
    auto rotation = glm::vec3(m_Pitch, m_Yaw, m_Roll);

    ImGui::DragFloat2("Resolution", &m_Resolution[0], 1.0f, 16.f, 4096.f, "%.4f");
    ImGui::DragFloat3("position XYZ", &m_CameraWorldPosition[0], 0.5f, -10000.f, 10000.f, "%.4f");
    ImGui::DragFloat3("rotation XYZ", &rotation[0], 0.5f, -360.f, 360.f, "%.4f");
    ImGui::DragFloat("field of view", &m_Fovy, 0.1f, 0.f, 120.f, "%.4f");
    ImGui::DragFloat("movement speed", &m_MovementSpeed, 0.5f, 1.0f, 10.f, "%.4f");
    ImGui::DragFloat("sensitivity", &m_Sensitivity, 0.01f, 0.001f, 1.f, "%.4f");
    ImGui::DragFloat("Near", &m_Near, 1.f, -10000, 10000, "%.4f");
    ImGui::DragFloat("Far", &m_Far, 1.f, -10000, 10000, "%.4f");

    // camera renderer
    ImGui::PushID("Color picker");
    ImVec4 color(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    
    ImGui::ColorPicker3(
        "Clear color", 
        (float*)&color, 
        ImGuiColorEditFlags_::ImGuiColorEditFlags_PickerHueBar  |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoSidePreview |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs      |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_NoAlpha
    );
    
    ImGui::ColorEdit4(
        "RGB",
        (float*)&color, 
        ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_InputHSV   |
        ImGuiColorEditFlags_::ImGuiColorEditFlags_Uint8);

    m_ClearColor = glm::vec4(color.x, color.y, color.z, color.w);

    ImGui::PopID();

    set_pitch(rotation.x);
    set_yaw(rotation.y);
    set_roll(rotation.z);
}

Component::TReturnType Camera::create()
{
    return std::make_unique<Camera>();
}

void Camera::render()
{
    if(get_object() == nullptr)
    {
        // TODO: add log here !!!
        return;
    }

    // try to get scene
    auto scene = get_object()->get_parent_recursive<Scene3D>();
    if(scene == nullptr) 
        scene = dynamic_cast<Scene3D*>(get_object());

    if(scene == nullptr) // no scene --> no rendering !!!
    {
        // TODO: add log here !!!
        return;
    }

    auto width  = m_Resolution.x;
    auto height = m_Resolution.y;

    // resize frame buffer
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)width, (int)height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, m_TextureDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)width, (int)height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_TextureDepthBuffer);

    // bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
    
    // depth/stencil/blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glViewport(0, 0, (int)width, (int)height);

    if(is_enabled())
    {
        // setup projection matrix to the scenes shaders
        auto projectionMatrix = get_projection_matrix();

        scene->apply_to_children_recursive(
            [this, &projectionMatrix](objectRef _Objcet)
            {
                auto component = _Objcet->get_component<IShader>();
                auto shader    = component != nullptr ? component->get_shader() : nullptr;

                if(shader == nullptr) 
                    return;

                shader->use();
                shader->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);
                shader->unuse();
            }
        );

        // process scene
        scene->frame_finish();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_FRAMEBUFFER, 0);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}