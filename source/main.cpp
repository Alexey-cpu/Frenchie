#include <FrenchieApplication.hpp>

#include <FrenchieRendererMeshRendererComponent.hpp>
#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererMesh2D.hpp>

// layers
#include <FrenchieApplicationCommandsQueueLayer.hpp>
#include <FrenchieApplicationHierarchyViewLayer.hpp>
#include <FrenchieApplicationInspectorViewLayer.hpp>
#include <FrenchieApplicationSceneViewLayer.hpp>
#include <FrenchieApplicationTimeProviderLayer.hpp>
#include <FrenchieApplicationCursorWatcherLayer.hpp>

#include <FrenchieImGuiDemoLayer.hpp>

#include <FrenchieCoreSerializationXMLFormat.hpp>

#include <FrenchieCoreSerializationTests.hpp>

using namespace Frenchie::Core;
using namespace Frenchie::Core::Serialization;
using namespace Frenchie::Renderer;
using namespace Frenchie::Application;

class CameraObject : public Object
{
public:
    CameraObject(const std::string& _Name) : Object(_Name)
    {
        // add_component<Camera>();
        // add_component<Transform>();

        //std::cout << "CameraObject::CameraObject " << get_name() << "\n";
    }

    virtual ~CameraObject()
    {
        //std::cout << "CameraObject::~CameraObject " << get_name() << "\n";
    }

    virtual void frame_update() override
    {
        Object::frame_update();

        auto camera    = get_component<Camera>();
        auto transform = get_component<Transform>();

        if(camera != nullptr && transform != nullptr) 
            camera->set_position(transform->get_world_position());
    }
};

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int, char**)
{
    // auto start = Helpers::tic();

    // Document doc;

    // auto root = doc.append_child("Root", "Zero");
    // doc.append_child("Root", "Zero");
    // doc.append_child("Root", "Zero");
    // doc.append_child("Root", "Zero");

    // int N = 3;

    // for (int i = 0; i < N; i++)
    // {
    //     auto child = root.append_child(
    //         fmt::format("Child_{}", i).c_str(), 
    //         Helpers::to_string<int>(i).c_str());

    //     for (int j = 0; j < N; j++)
    //     {
    //         child = child.append_child(
    //             fmt::format("Child_{}_{}", i, j).c_str(), 
    //             Helpers::to_string<int>(i).c_str());
    //     } 
    // }

    // std::cout << "file build time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    // start = Helpers::tic();

    // doc.write<XML<false>>("C:/SDK/Qt_Projects/OpenGL/logs/TestFile.xml");

    // std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    // start = Helpers::tic();

    // doc.read<XML<false>>("C:/SDK/Qt_Projects/OpenGL/logs/TestFile.xml");

    // std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    // doc.write<XML<false>>("C:/SDK/Qt_Projects/OpenGL/logs/TestFile1.xml");

    // //---------------------------------------------------------------------------------------------------------------------
    // WRAPPER VERSION
    //---------------------------------------------------------------------------------------------------------------------
    // auto start = Helpers::tic();
    // auto doc = Serialization::Format<Serialization::XML>::read("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    // std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    // start = Helpers::tic();
    // Serialization::Format<Serialization::XML>::write(doc.get(), "C:/SDK/Qt_Projects/OpenGL/logs/NewDocument1.xml");
    // std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    //---------------------------------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------------------------------
    // HIGHLY OPTIMIZED VARIANT
    //---------------------------------------------------------------------------------------------------------------------
    // //file read time
    // Document doc;
    // doc.read("C:/SDK/Qt_Projects/OpenGL/logs/NewFile.xml");
    // doc.write("C:/SDK/Qt_Projects/OpenGL/logs/NewFile2.xml");
    // for(auto&& node : doc.nodes) std::cout << node.self() << "\t" << node.parent().self() << "\t" << node.name() << "\n";

    // auto start = Helpers::tic();
    // Document doc;

    // Format<XML<false>>::read(doc, "C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    // std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    // start = Helpers::tic();
    // Format<XML<false>>::write(doc, "C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML1.pwrct");    
    // std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    //---------------------------------------------------------------------------------------------------------------------
    // FAST FILE READ
    //---------------------------------------------------------------------------------------------------------------------
    // read the whole file
    // auto start = Helpers::tic();

    // std::ifstream t("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");

    // t.seekg(0, std::ios::end);
    // size_t size = t.tellg();
    // std::string buffer(size, ' ');
    // t.seekg(0);
    // t.read(&buffer[0], size);

    // std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    //---------------------------------------------------------------------------------------------------------------------

    Document pool;

    auto start = Helpers::tic();

    pool.read<Format<XML_COMPACT>>("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");

    std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    start = Helpers::tic();

    pool.write<Format<XML_COMPACT>>("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML_Copy.pwrct");

    std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    return 0;
}

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