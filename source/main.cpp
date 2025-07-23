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

class document final
{
public:

    template<typename T>
    class stack
    {
    public:

        stack()
        {
            container.resize(512);
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
        node(const document* _Document) : 
            document(_Document){}

        friend class document;

    public:
        
        ~node(){}

        const document* document  = nullptr;
        std::string     name      = std::string();
        std::string     value     = std::string();
        int             self      = 0;
        int             parent    = 0;

        node* append_child(const char* _Name, const char* _Value)
        {
            if(document == nullptr) 
                return nullptr;

            return document->create_node(this, _Name, _Value);
        }
    };


    document(){}

    ~document()
    {
        clear();
    }

    mutable std::vector<node*> nodes;

    void clear()
    {
        for(auto&& node : nodes) 
            delete node;
        nodes.clear();
    }

    node* root() const
    {
        return nodes[0];
    }

    void read(const std::filesystem::path& _Path)
    {
        // load file
        pugi::xml_document doc;
        auto status = doc.load_file(_Path.c_str()).status;

        if(status != pugi::xml_parse_status::status_ok)
        {
            status = doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status;

            if(status != pugi::xml_parse_status::status_ok) 
                return;
        }

        if(doc.empty()) 
            return;

        struct Element
        {
            pugi::xml_node xml;
            node*          data;
        };

        // clear self
        clear();

        // parse in depth
        stack<Element> stack;
        stack.push({doc, nullptr});

        while(!stack.empty())
        {
            auto top = stack.top();
            stack.pop();

            for(auto&& element : top.xml)
            {
                stack.push(
                    {
                        element, 
                        create_node(top.data, element.name(), element.value())
                    }
                );
            }
        }
    }

    bool write(const std::filesystem::path& _Path)
    {
        // retrieve children
        std::vector<std::vector<node*>> parents(nodes.size());
        for(auto&& item : nodes) 
        {
            if(item->self != item->parent)
                parents[item->parent].push_back(nodes[item->self]);
        }

        // write to file
        struct Element
        {
            pugi::xml_node xml;
            node*          data;
        };

        pugi::xml_document  main;
        std::queue<Element> queue;
        queue.push({main, root()});

        while (!queue.empty())
        {
            auto data = queue.front().data;
            auto xml  = queue.front().xml;
            queue.pop();

            auto node = xml.append_child(data->name);

            for(auto&& child : parents[data->self]) 
                queue.push({node, child});
        }

        return main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw);
    }

protected:

    node* create_node(node* _Parent, const char* _Name, const char* _Value) const
    {
        auto item    = new node(this);
        item->name   = _Name;
        item->value  = _Value;
        item->self   = std::max<int>((int)nodes.size(), 0);
        item->parent = _Parent != nullptr ? _Parent->self : 0;
        nodes.push_back(item);
        return item;
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

    // //file read time
    // document doc;
    // doc.read("C:/SDK/Qt_Projects/OpenGL/logs/NewFile.xml");
    // doc.write("C:/SDK/Qt_Projects/OpenGL/logs/NewFile1.xml");
    // for(auto&& node : doc.nodes) std::cout << node->self << "\t" << node->parent << "\t" << node->name << "\n";

    auto start = Helpers::tic();
    document doc;
    doc.read("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    start = Helpers::tic();
    doc.write("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML1.pwrct");    
    std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    

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