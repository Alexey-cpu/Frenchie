#include <FrenchieApplication.hpp>
#include <FrenchieApplicationSceneViewLayer.hpp>

#include <FrenchieCoreFlyweight.hpp>

#include <FrenchieRendererMesh.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Renderer;
using namespace Frenchie::Application;

// int main(int, char**)
// {
//     Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

//     auto shader = 
//     Flyweight::instance()->request<Shader>(
//         "Frenchie/Shader/Default",
//         std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert"),
//         std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")
//     );

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
    auto scene = create_raw_pointer<Scene3D>();

    Mesh* mesh = AssetManager::instance()->request<Triangle2D>("Frenchie/Mesh/Triangle2D");

    // create shader
    Shader* shader = nullptr;

    for(int i = 0; i < 1e3; i++)
    {
        shader = 
            AssetManager::instance()->request<Shader>(
                "Frenchie/Shader/Default",
                std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert"),
                std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag")
            );
    }

    // create hierarchy
    auto root    = new MeshRenderer(mesh, shader, "Root", scene);
    auto child_1 = new MeshRenderer(mesh, shader, "Child-1", root);
    auto child_2 = new MeshRenderer(mesh, shader, "Child-2", child_1);
    auto child_3 = new MeshRenderer(mesh, shader, "Child-3", child_2);

    auto child_4 = new MeshRenderer(mesh, shader, "Child-4");
    auto child_5 = new MeshRenderer(mesh, shader, "Child-5");
    auto child_6 = new MeshRenderer(mesh, shader, "Child-6");

    root->set_position(glm::vec3(0.1f, 0.0f, 0.f));
    root->set_rotation(glm::vec3(0.f, 0.f, 0.f));
    child_1->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_2->set_position(glm::vec3(200.f, 200.f, 0.f));
    child_3->set_position(glm::vec3(200.f, 200.f, 0.f));

    application->push<SceneView>("RenderingTest", scene);

    return application->execute();
}