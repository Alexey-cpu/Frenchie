#include <FrenchieApplication.hpp>
#include <FrenchieApplicationSceneViewLayer.hpp>
#include <FrenchieApplicationHierarchyViewLayer.hpp>
#include <FrenchieApplicationInspectorViewLayer.hpp>

#include <FrenchieCoreFlyweight.hpp>

#include <FrenchieRendererMeshRendererComponent.hpp>
#include <FrenchieRendererTransformComponent.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;
using namespace Frenchie::Application;

int main(int, char**)
{
    // setup logger
    Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

    // setup application
    auto application = Frenchie::Application::Application::instance();
    application->set_window_size(glm::vec2(2048, 1024));
    application->set_maximized(true);

    // push application layers
    auto scene = std::make_shared<Scene3D>();

    Mesh* mesh = AssetManager::instance()->request<Triangle2D>("Frenchie/Mesh/Triangle2D");

    // create shader
    Shader* shader = nullptr;

    auto shaderPath = std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared");

    Logger::instance()->info("---------------------------------------------------------------------------------");
    Logger::instance()->info(fmt::format("shader path: {}", shaderPath.string()));
    Logger::instance()->info("---------------------------------------------------------------------------------");

    for(int i = 0; i < 1e3; i++)
    {
        shader = 
            AssetManager::instance()->request<Shader>(
                "Frenchie/Shader/Default",
                shaderPath.string().append("/shaders/Default/Default.vert"),
                shaderPath.string().append("/shaders/Default/Default.frag")
            );
    }

    // create hierarchy
    auto root    = scene->create_child<Object>("Root");
    auto child_1 = root->create_child<Object>("Child-1");
    auto child_2 = child_1->create_child<Object>("Child-2");
    auto child_3 = child_2->create_child<Object>("Child-3");

    scene->apply_to_children_recursive([mesh, shader](Object* _Object)
    {
        auto component = 
            Factory::create<Component>(STRINGIFY(Transform));
        
        if(component) std::cout << "Component created !!! \n";

        _Object->add_component<Transform>();
        _Object->add_component<MeshRenderer>(mesh, shader);
    }
    );

    root->get_component<Transform>()->set_position(glm::vec3(0.f, 0.0f, 0.f));
    root->get_component<Transform>()->set_rotation(glm::vec3(0.f, 0.f, 0.f));
    child_1->get_component<Transform>()->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_2->get_component<Transform>()->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_3->get_component<Transform>()->set_position(glm::vec3(200.f, 200.f, 0.f));


    application->push<SceneView>("Scene", scene);
    application->push<HierarchyView>("Hierarchy", scene);
    application->push<InspectorView>("Inspector", scene);

    return application->execute();
}