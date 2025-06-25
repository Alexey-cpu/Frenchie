#include <FrenchieRendererOpenGLScene.hpp>
#include <FrenchieRendererOpenGLCamera.hpp>
#include <FrenchieRendererOpenGLShader.hpp>

#include <FrenchieCoreFlyweight.hpp>

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

    // setup shader
    auto shader = Frenchie::Core::FlyweightFactory::instance()->Request<ShaderProgram>();

    auto viewportscale = viewport->get_viewport_scale();

    shader->begin();
    shader->set_uniform<glm::vec3>("u_ViewportScale", viewport->get_viewport_scale());
    shader->set_uniform<glm::mat4>("u_ViewMatrix", camera->get_view_matrix());
    shader->set_uniform<glm::mat4>("u_ProjectionMatrix", viewport->get_projection_matrix());
    shader->end();

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