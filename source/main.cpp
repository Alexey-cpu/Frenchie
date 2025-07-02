#include <FrenchieApplication.hpp>
#include <FrenchieApplicationSceneViewLayer.hpp>
#include <FrenchieApplicationHierarchyViewLayer.hpp>
#include <FrenchieApplicationInspectorViewLayer.hpp>

#include <FrenchieCoreFlyweight.hpp>

#include <FrenchieRendererMeshRendererComponent.hpp>
#include <FrenchieRendererTransformComponent.hpp>

#include <FrenchieImGuiDemoLayer.hpp>
#include <FrenchieMeshRenderer2D.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;
using namespace Frenchie::Application;

// int main(int, char**)
// {
//     Rectangle2D rect;

//     return 0;
// }


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

    auto mesh = Cache<Rectangle2D>::request("Frenchie/Mesh/Triangle2D");

    // create shader
    auto shaderPath = std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared");

    Logger::instance()->info("---------------------------------------------------------------------------------");
    Logger::instance()->info(fmt::format("shader path: {}", shaderPath.string()));
    Logger::instance()->info("---------------------------------------------------------------------------------");

    auto shader = 
        Cache<Shader>::request(
            "Frenchie/Shader/Default",
            shaderPath.string().append("/shaders/Default/Default.vert"),
            shaderPath.string().append("/shaders/Default/Default.frag")
        );

    // create a root object
    auto root = scene->create_child<Object>(fmt::format("Root"));
    root->add_component<Transform>();
    root->add_component<MeshRenderer>(mesh, shader);
    root->get_component<Transform>()->set_position(glm::vec3(0.f, 0.f, 0.f));

    // // create child objects
    // for(int i = 1; i < 1e1; i++)
    // {
    //     auto item = root->create_child<Object>(fmt::format("Item-{}", i));
    //     item->add_component<Transform>();
    //     item->add_component<MeshRenderer>(mesh, shader, aabb);
    //     item->get_component<Transform>()->set_position(glm::vec3(i * 200, i * 200, 0.f));
    // }

    // create application layers
    application->push<SceneView>("Scene", scene);
    application->push<HierarchyView>("Hierarchy", scene);
    application->push<InspectorView>("Inspector", scene);
    application->push<ImguiDemo>();

    return application->execute();
}