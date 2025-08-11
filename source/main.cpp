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

#include <FrenchieCoreChunkAllocator.hpp>

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

// Memory pool test
int main(int, char**)
{
    // 4-element allocator
    const int CunkSize   = 4;
    const int CunksCount = 10000;

    Frenchie::Core::MemoryChunkAllocator<int, CunkSize> allocator;

    // allocate 4 element
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);

    // allocate 4 element
    auto p0 = allocator.allocate(1);
    auto p1 = allocator.allocate(1);
    auto p2 = allocator.allocate(1);
    auto p3 = allocator.allocate(1);

    // allocate 4 element
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);

    // allocate 4 element
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);

    // allocate 4 element
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);
    allocator.allocate(1);

    allocator.deallocate(p0);
    allocator.deallocate(p1);
    allocator.deallocate(p2);
    allocator.deallocate(p3);

    return 0;
}

// Serialiation tool test
// int main(int, char**)
// {
//     Document document;

//     auto root    = document.append_node("Name", "Root");
//     auto scalars = root.append_node("Scalars", "");
//     auto vectors = root.append_node("Vectors", "");
//     auto lists   = root.append_node("Lists", "");
//     auto sets    = root.append_node("Sets", "");

//     #define __test_append_value__(__node, __type, __value) __node.append_value_node<__type>("Value", __value);
//     #define __test_append_vector__(__node, __type, __value) __node.append_value_node<std::vector<__type>>("Value", std::vector<__type>({__value, __value, __value}));
//     #define __test_append_list__(__node, __type, __value) __node.append_value_node<std::list<__type>>("Value", std::list<__type>({__value, __value, __value}));
//     #define __test_append_set__(__node, __type, __value) __node.append_value_node<std::set<__type>>("Value", std::set<__type>({__value, __value, __value}));

//     __test_append_value__(scalars, bool, false)
//     __test_append_value__(scalars, float, 1.5f)
//     __test_append_value__(scalars, double, 1.12313)
//     __test_append_value__(scalars, int, 1000)
//     __test_append_value__(scalars, unsigned int, 2000)
//     __test_append_value__(scalars, long, 2000)
//     __test_append_value__(scalars, unsigned long, 2000)
//     __test_append_value__(scalars, long long, 2000)
//     __test_append_value__(scalars, unsigned long long, 2000)

//     __test_append_vector__(vectors, bool, false)
//     __test_append_vector__(vectors, float, 1.5f)
//     __test_append_vector__(vectors, double, 1.12313)
//     __test_append_vector__(vectors, int, 1000)
//     __test_append_vector__(vectors, unsigned int, 2000)
//     __test_append_vector__(vectors, long, 2000)
//     __test_append_vector__(vectors, unsigned long, 2000)
//     __test_append_vector__(vectors, long long, 2000)
//     __test_append_vector__(vectors, unsigned long long, 2000)

//     __test_append_list__(lists, bool, false)
//     __test_append_list__(lists, float, 1.5f)
//     __test_append_list__(lists, double, 1.12313)
//     __test_append_list__(lists, int, 1000)
//     __test_append_list__(lists, unsigned int, 2000)
//     __test_append_list__(lists, long, 2000)
//     __test_append_list__(lists, unsigned long, 2000)
//     __test_append_list__(lists, long long, 2000)
//     __test_append_list__(lists, unsigned long long, 2000)

//     __test_append_set__(sets, bool, false)
//     __test_append_set__(sets, float, 1.5f)
//     __test_append_set__(sets, double, 1.12313)
//     __test_append_set__(sets, int, 1000)
//     __test_append_set__(sets, unsigned int, 2000)
//     __test_append_set__(sets, long, 2000)
//     __test_append_set__(sets, unsigned long, 2000)
//     __test_append_set__(sets, long long, 2000)
//     __test_append_set__(sets, unsigned long long, 2000)

//     #undef __append_value__
//     #undef __append_vector__
//     #undef __test_append_set__
//     #undef __test_append_list__
        
//     for(auto&& node : scalars) 
//         std::cout << node.name() << "\t" << node.value() << "\n";

//     for(auto&& vector : vectors) 
//     {
//         std::cout << vector.name() << "\t" << vector.value() << "\n";
//         for(auto&& item : vector) 
//             std::cout << item.name() << "\t" << item.value() << "\n";
//     }

//     for(auto&& vector : lists) 
//     {
//         std::cout << vector.name() << "\t" << vector.value() << "\n";
//         for(auto&& item : vector) 
//             std::cout << item.name() << "\t" << item.value() << "\n";
//     }

//     for(auto&& vector : sets) 
//     {
//         std::cout << vector.name() << "\t" << vector.value() << "\n";
//         for(auto&& item : vector) 
//             std::cout << item.name() << "\t" << item.value() << "\n";
//     }

//     document.write<Format<XML_BEAUTIFUL>>("C:/SDK/Qt_Projects/OpenGL/logs/values_node_test.xml");
//     document.read<Format<XML_BEAUTIFUL>>("C:/SDK/Qt_Projects/OpenGL/logs/values_node_test.xml");
//     document.write<Format<XML_BEAUTIFUL>>("C:/SDK/Qt_Projects/OpenGL/logs/values_node_test_copy.xml");

//     return 0;
// }

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