
// SCVFramework
#include <FrenchieRendererScene.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererViewport.hpp>

// OpenGL
#include <FrenchieRendererOpenGLShader.hpp>

// Core
#include <FrenchieCoreFlyweight.hpp>

#include <FrenchieCoreLogger.hpp>

using namespace Frenchie::Renderer;
using namespace Frenchie::Renderer::OpenGL;

Scene::Scene(
    const std::string&      _Name, 
    Frenchie::Core::Object* _Parent) : 
    Frenchie::Core::Object(_Name, _Parent){}

Scene::~Scene(){}

// virtual API
bool Scene::awake()
{
    return Frenchie::Core::Object::awake();
}

void Scene::frame_start()
{
    Viewport* viewport = 
        get_parent_recursive<Viewport>();

    Camera* camera = 
        viewport != nullptr ? 
            viewport->find_child<Camera>() : 
                nullptr;

    if(viewport == nullptr || camera == nullptr) 
        return;

    auto projectionMatrix = viewport->get_projection_matrix();
    auto viewportScale    = viewport->get_viewport_scale();
    auto viewMatrix       = camera->get_view_matrix();

    Frenchie::Core::FlyweightFactory::instance()->apply_to_all_instances(
        [&projectionMatrix, &viewportScale, &viewMatrix](std::any _Instance)
        {
            try
            {
                Shader* shader = std::any_cast<Shader*>(_Instance);

                shader->begin();
                shader->set_uniform<glm::mat4>("u_ProjectionMatrix", projectionMatrix);
                shader->set_uniform<glm::vec3>("u_ViewportScale", viewportScale);
                shader->set_uniform<glm::mat4>("u_ViewMatrix", viewMatrix);
                shader->end();
            }
            catch(...)
            {
            }
        }
    );

    Frenchie::Core::Object::frame_start();
}

void Scene::frame_update()
{
    Frenchie::Core::Object::frame_update();
}

void Scene::frame_finish()
{
    Frenchie::Core::Object::frame_finish();
}