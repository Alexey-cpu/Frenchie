#include <FrenchieImGuiOpenGLScene.hpp>

#include <FrenchieApplication.hpp>

#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererOpenGLMeshRenderer.hpp>
#include <FrenchieRendererOpenGLRectMesh.hpp>

#include <FrenchieCoreFlyweight.hpp>
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreLogger.hpp>

#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Renderer;
using namespace Frenchie::Renderer::OpenGL;

Scene::Scene(const std::string& _Name) : Layer(_Name)
{
    // create viewport
    m_Viewport = create_shared_pointer<Scene3D>(
        100.f,
        1.f,
        90.f,
        glm::vec3(1.f, 1.f, 1.f));

    // create camera
    create_raw_pointer<Camera>(
        glm::vec3(+0.f, +0.f, +1.f),
        glm::vec3(+0.f, +1.f, +0.f),
        "Camera",
        m_Viewport.get()
    );
}

Scene::~Scene(){}

bool Scene::awake() 
{
    RectMesh* mesh = FlyweightFactory::instance()->Create<RectMesh>();

    // create shader
    Shader* shader = 
        FlyweightFactory::instance()->Create<Shader>(
            std::vector<std::shared_ptr<ShaderLoader>>(
            {
                create_shared_pointer<ShaderLoader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert")),
                create_shared_pointer<ShaderLoader>(std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")),
            }
        )
    );

    // create hierarchy
    auto root    = new MeshRenderer(mesh, shader, "Root", m_Viewport.get());
    auto child_1 = new MeshRenderer(mesh, shader, "Child-1", root);
    auto child_2 = new MeshRenderer(mesh, shader, "Child-2", child_1);
    auto child_3 = new MeshRenderer(mesh, shader, "Child-3", child_2);

    root->set_position(glm::vec3(0.1f, 0.0f, 0.f));
    root->set_rotation(glm::vec3(0.f, 0.f, 0.f));
    child_1->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_2->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_3->set_position(glm::vec3(200.f, 200.f, 0.f));

    return m_Viewport->awake() && m_SceneScreenshooter.awake();
}

void Scene::frame_start()
{
    m_Viewport->frame_start();
}

void Scene::frame_update()
{
    ImGui::Begin(get_name().c_str());

    m_Viewport->set_size(
        glm::vec2(
            ImGui::GetContentRegionAvail().x, 
            ImGui::GetContentRegionAvail().y)
        );

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::GetWindowDrawList()->AddImage(
        m_SceneScreenshooter.textureColorbuffer, 
        ImVec2(pos.x, pos.y), 
        ImVec2(pos.x + m_Viewport->get_size().x, pos.y + m_Viewport->get_size().y), 
        ImVec2(0, 1), 
        ImVec2(1, 0)
    );


    ImGui::End();

    m_Viewport->frame_update();
}

void Scene::frame_finish()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_SceneScreenshooter.framebuffer);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Viewport->apply_to_children_recursive(
        [](Object* _Object)
        {
            Transform* transform = 
                dynamic_cast<Transform*>(_Object);

            if(transform == nullptr || dynamic_cast<Camera*>(_Object) != nullptr) 
                return;

            transform->set_rotation(
                glm::vec3(
                    0.f,
                    0.f,
                    glm::degrees(2.f * glm::pi<float>() * 0.1f * (float)glfwGetTime())
                )
            );
        }
    );

    m_Viewport->frame_finish();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Scene::finish()
{
    // dispose here
    Layer::finish();
}

void Scene::close()
{
    // handle
    Layer::close();
}

bool Scene::is_closed()
{
    return Layer::is_closed();
}