#include <FrenchieApplication.hpp>

#include <FrenchieImGuiDemoLayer.hpp>
#include <FrenchieApplicationEditor.hpp>
#include <FrenchieApplicationEditorConsoleLayer.hpp>

// #include <FrenchieRendererMeshRendererComponent.hpp>
// #include <FrenchieRendererTransformComponent.hpp>
// #include <FrenchieRendererMesh2D.hpp>

// // layers
// #include <FrenchieApplicationCommandsQueueLayer.hpp>
// #include <FrenchieApplicationHierarchyViewLayer.hpp>
// #include <FrenchieApplicationInspectorViewLayer.hpp>
// #include <FrenchieApplicationSceneViewLayer.hpp>
// #include <FrenchieApplicationTimeProviderLayer.hpp>
// #include <FrenchieApplicationCursorWatcherLayer.hpp>

// #include <FrenchieCoreSerializationFormatXML.hpp>
// #include <FrenchieCoreSerializationFormatJSON.hpp>
// #include <FrenchieCoreSerializationFormatYAML.hpp>

// #include <FrenchieCoreSerializationTests.hpp>

// #include <FrenchieCoreChunkAllocator.hpp>

// using namespace Frenchie::Core;
// using namespace Frenchie::Core::Serialization;
// using namespace Frenchie::Renderer;
// using namespace Frenchie::Application;

// class CameraObject : public Object
// {
// public:
//     CameraObject(const std::string& _Name) : Object(_Name)
//     {
//         add_component<Camera>();
//         add_component<Transform>();

//         //std::cout << "CameraObject::CameraObject " << get_name() << "\n";
//     }

//     virtual ~CameraObject()
//     {
//         //std::cout << "CameraObject::~CameraObject " << get_name() << "\n";
//     }

//     virtual void frame_update() override
//     {
//         Object::frame_update();

//         auto camera    = get_component<Camera>();
//         auto transform = get_component<Transform>();

//         if(camera != nullptr && transform != nullptr) 
//             camera->set_position(transform->get_world_position());
//     }
// };

// #include "rapidjson/document.h"
// #include "rapidjson/prettywriter.h"
// #include "rapidjson/stringbuffer.h"
// #include "rapidjson/filewritestream.h"
// #include <rapidjson/writer.h>

// #include <fstream>
// #include <iostream>
// #include <sstream>

// int main(int, char**)
// {
//     // setup logger
//     Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

//     // setup application
//     auto application = Frenchie::Application::Application::instance();
//     application->set_window_size(glm::vec2(2048, 1024));
//     application->set_maximized(true);

//     // push application layers
//     auto mesh = Cache<Rectangle2D>::request("Frenchie/Mesh/Triangle2D");

//     // create shader
//     auto shaderPath = std::filesystem::path("C:/SDK/Qt_Projects/OpenGL/shared");

//     Logger::instance()->info("---------------------------------------------------------------------------------");
//     Logger::instance()->info(fmt::format("shader path: {}", shaderPath.string()));
//     Logger::instance()->info("---------------------------------------------------------------------------------");

//     auto shader = 
//         Cache<Shader>::request(
//             "Frenchie/Shader/Default",
//             shaderPath.string().append("/shaders/Default/Default.vert"),
//             shaderPath.string().append("/shaders/Default/Default.frag")
//         );

//     // create scene
//     auto scene = std::make_shared<Scene3D>();
//     scene->add_component<Camera>(glm::vec3(+0.f, +0.f, +10000.f), glm::vec3(+0.f, +1.f, +0.f));
//     //scene->add_component<Scene3DBHV>();

//     // create a root object
//     auto root = scene->create_child<CameraObject>(fmt::format("Root"));
//     root->add_component<Transform>();
//     root->add_component<MeshRenderer>(mesh, shader);
//     root->get_component<Transform>()->set_position(glm::vec3(0.f, 0.f, 0.f));

//     //create child objects
//     float radius  = 600.f;
//     int   counter = 0;
//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(mesh, shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             std::sin(i) * radius, 
//             0.f
//             )
//         );
//     }

//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(mesh, shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             0.f, 
//             std::sin(i) * radius
//             )
//         );
//     }

//     for(float i = 0; i < 2.f * glm::pi<float>(); i += 2.f * glm::pi<float>() / 10.f)
//     {
//         auto item = root->create_child<CameraObject>(fmt::format("Item-{}", counter++));
//         //item->add_component<Transform>();
//         item->add_component<MeshRenderer>(Cache<Rectangle2D>::request("Frenchie/Mesh/Triangle2D1"), shader);
//         item->get_component<Transform>()->set_position(
//             glm::vec3(std::cos(i) * radius, 
//             400.f, 
//             std::sin(i) * radius
//             )
//         );
//     }

//     // create application layers
//     application->push<CommandsQueueLayer>()->push<CallbackCommand>(
//         [scene]()
//         {
//             if(scene != nullptr) 
//                 scene->awake();
//         }
//     );

//     application->push<TimeProviderLayer>();
//     application->push<CursorWatcher>();

//     application->push<SceneView>("Scene-1", scene);
//     //application->push<SceneView>("Scene-2", scene);
//     application->push<HierarchyView>("Hierarchy", scene);
//     application->push<InspectorView>("Inspector", scene);
//     application->push<ImguiDemo>();

//     return application->execute();
// }

void parseMainMenuTree(
    Frenchie::Core::Serialization::Node& _Parent, 
    std::vector<std::string>&            _Items)
{
    struct Element
    {
        Frenchie::Core::Serialization::Node document;
        std::string                         hierarchy;
    };

    for(auto&& item : _Items)
    {
        Frenchie::Core::Helpers::Stack<Element> stack;
        stack.push({_Parent, item});

        while(!stack.empty())
        {
            auto document  = stack.top().document;
            auto hierarchy = stack.top().hierarchy;
            stack.pop();

            auto contents = Frenchie::Core::Helpers::String::split(hierarchy, "::");

            if(contents.empty()) 
                continue;
            
            // parse hierarchy
            hierarchy.clear();

            for (size_t i = 1; i < contents.size(); i++)
            {
                hierarchy = hierarchy.append(contents[i]);

                if(i < contents.size() - 1) 
                    hierarchy.append("::");
            }

            // push node onto stack
            auto node = document.find_node(contents.front().c_str());
            if(!node.is_valid())
                node = document.append_node(contents.front().c_str());

            stack.push({node, hierarchy});
        }
        
    }
}

int main(int, char**)
{
    return Frenchie::Application::Editor::Editor().execute();
}