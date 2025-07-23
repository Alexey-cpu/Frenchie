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
using namespace Frenchie::Renderer;
using namespace Frenchie::Application;

class CameraObject : public Object
{
public:
    CameraObject(const std::string& _Name) : Object(_Name)
    {
        add_component<Camera>();
        add_component<Transform>();
    }

    virtual ~CameraObject(){}

    virtual void frame_update() override
    {
        Object::frame_update();

        auto camera    = get_component<Camera>();
        auto transform = get_component<Transform>();

        if(camera != nullptr && transform != nullptr) 
            camera->set_position(transform->get_world_position());
    }
};

//------------------------------------------------------------------------------------------------
// EXPERIMENTAL
//------------------------------------------------------------------------------------------------
// char buffer[512];
// std::pmr::monotonic_buffer_resource monotonicResource(buffer, sizeof(buffer));
// std::pmr::pool_options options{1, 1024 * 1024 * 1024};
// std::pmr::unsynchronized_pool_resource poolResource(options, &monotonicResource);

class document;
class node;

template<typename T>
class SStack
{
public:

    SStack()
    {
        container.resize(128);
    }

    std::vector<T> container;

    void push(const T& _Value)
    {
        current           = next;
        container[next++] = _Value;

        if(next >= container.size()) 
        {
            container.resize(growth * container.size());
            growth *= 2;
        }
    }
    
    void pop()
    {
        next--;
        current--;
    }

    T& top()
    {
        return container[current];
    }

    bool empty() const
    {
        return current < 0;
    }

    int current = 0;
    int next    = 0;
    int growth  = 2;
};

class node final
{
public:
    
    node(){}

    ~node(){}

    std::string name;
    std::string value;
    int         self;
    int         parent;
};

class document
{
public:

    virtual ~document()
    {
        for(auto&& node : nodes) 
            delete node;
    }

    std::vector<node*> nodes;
    pugi::xml_document doc;

    node* push(const int& index = 0, const char* name = nullptr, const char* value = nullptr)
    {
        nodes.push_back(new node());
        nodes.back()->name   = name;
        nodes.back()->value  = value;
        nodes.back()->self   = std::max<int>((int)nodes.size() - 1, 0);
        nodes.back()->parent = index;

        return nodes.back();
    }

    node* push(node* _Parent, const char* name = nullptr, const char* value = nullptr)
    {
        return push(_Parent->self, name, value);
    }

    void read(const std::filesystem::path& _Path)
    {
        // load file
        auto status = doc.load_file(_Path.c_str()).status;

        if(status != pugi::xml_parse_status::status_ok)
        {
            status = doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status;

            if(status != pugi::xml_parse_status::status_ok) 
                return;
        }

        if(doc.empty()) 
            return;

        nodes.clear();
        auto root = push(0, doc.name(), doc.value());
            
        SStack<std::pair<pugi::xml_node, node*>> stack;
        stack.push({doc, root});

        while(!stack.empty())
        {
            auto top = stack.top();
            stack.pop();

            for(auto&& element : top.first)
            {
                stack.push(
                    {
                        element,
                        push(top.second, element.name(), element.text().get())
                    }
                );
            }
        }
    }
};
//------------------------------------------------------------------------------------------------

int main(int, char**)
{
    Serialization::Tests::SerializationTests tests("C:/SDK/Qt_Projects/OpenGL/logs");
    tests.run();

    // //file write time
    // auto document = 
    //     std::make_shared<Serialization::Node>("NewDocument");
    // auto start = Helpers::tic();
    // auto child = document.get();
    
    // for(int i = 0; i < 1e6; i++) 
    // {
    //     child = child->append_child("Child", 1);
    // }

    // std::cout << "doc generation time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    // auto start1 = Helpers::tic();
    // Serialization::Format<Serialization::XML>::write(document.get(), "C:/SDK/Qt_Projects/OpenGL/logs/NewDocument.xml");
    // std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start1, Helpers::tic()) << " ms \n";
    // std::cout << "total time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

    //file read time
    auto start2 = Helpers::tic();
    
    // document doc;
    // doc.read("C:/SDK/Qt_Projects/OpenGL/logs/NewFile.xml");
    // for(auto&& node : doc.nodes) std::cout << node->self << "\t" << node->parent << "\t" << node->name << "\n";

    document doc;
    doc.read("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    
    // auto instance = Serialization::Format<Serialization::XML>::read("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start2, Helpers::tic()) << " ms \n";

    // for(auto node : nodes) 
    //     delete node;

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