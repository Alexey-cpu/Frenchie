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

class Document final
{
public:

    // nested types
    template<typename T>
    class Stack final
    {
    public:

        Stack()
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

    template<typename T>
    class Queue final
    {
    public:

        Queue()
        {
            container.resize(512);
        }

        std::vector<T> container;

        void push(const T& _Value)
        {
            container[tail++] = _Value;

            if(tail >= container.size()) 
            {
                container.resize(growth * container.size());
                growth *= 2;
            }
        }
        
        void pop()
        {
            head++;
        }

        T& front()
        {
            return container[head];
        }

        bool empty() const
        {
            return head >= tail;
        }

        int head    = 0;
        int tail    = 0;
        int growth  = 2;
    };

    class Node final
    {
    public:
        
        ~Node(){}

        std::string name   = std::string();
        std::string value  = std::string();
        int         self   = 0;
        int         parent = 0;

        Node* append_child(const char* _Name, const char* _Value)
        {
            if(document == nullptr) 
                return nullptr;

            return document->append_node(this, _Name, _Value);
        }

    private:
        Node(const Document* _Document) : 
            document(_Document){}

        const Document* document = nullptr;

        friend class Document;
    };


    Document(){}

    ~Document()
    {
        clear();
    }

    void clear()
    {
        for(auto&& node : nodes) 
            delete node;
        nodes.clear();
    }

    Node* root() const
    {
        return nodes.empty() ? nullptr : nodes[0];
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
            pugi::xml_node document;
            Node*          node;
        };

        // clear self
        clear();

        // parse in depth
        Stack<Element> stack;
        stack.push({doc, nullptr});

        while(!stack.empty())
        {
            auto top = stack.top();
            stack.pop();

            for(auto&& element : top.document)
            {
                stack.push(
                    {
                        element, 
                        append_node(top.node, element.name(), element.value())
                    }
                );
            }
        }
    }

    bool write(const std::filesystem::path& _Path)
    {
        //----------------------------------------------------------------------------------------
        // THIS IS OPTIMIZED IMPLEMENTATION
        //----------------------------------------------------------------------------------------
        std::vector<Node*> items(nodes.size());
        std::vector<int>   pointers(nodes.size() + 1);
        std::vector<int>   workspace(nodes.size() + 1);

        for (size_t i = 0; i < nodes.size(); i++)
        {
            items[i]     = nodes[i];
            pointers [i] = 0;
            workspace[i] = 0;
        }

        pointers[nodes.size()] = 0;
        workspace[nodes.size()] = 0;

        for(auto&& item : nodes) 
        {
            if(item->self != item->parent)
            {
                pointers[item->parent]++;
                workspace[item->parent]++;
            }
        }

        // cumulative sum
        for( int i = 0, j = 0, k = 0 ; i < nodes.size() + 1; i++ )
        {
            k += workspace[i];
            workspace[i] = j;
            pointers[i] = j;
            j = k;
        }

        // count sort
        for(int i = 0; i < nodes.size(); i++ )
        {
            if(nodes[i]->self == nodes[i]->parent) 
                continue;

            int index    = workspace[nodes[i]->parent]++;
            items[index] = nodes[i];
        }

        //------------------------------------------------------------------------------
        // write to file
        struct Element
        {
            pugi::xml_node xml;
            Node*          data;
        };

        pugi::xml_document  main;
        Queue<Element>      queue;
        queue.push({main, root()});

        while (!queue.empty())
        {
            auto data = queue.front().data;
            auto xml  = queue.front().xml;
            queue.pop();

            auto node = xml.append_child(data->name);

            for (size_t i = pointers[data->self]; i < pointers[data->self+1]; i++) 
                queue.push({node, items[i]});
        }

        return main.save_file(pugi::as_utf8(_Path.wstring()).c_str());

        //----------------------------------------------------------------------------------------
        // THIS IS NAIVE IMPLEMENTATION
        //----------------------------------------------------------------------------------------

        // // retrieve children
        // std::vector<std::vector<Node*>> parents(nodes.size());
        // for(auto&& item : nodes) 
        // {
        //     if(item->self != item->parent)
        //         parents[item->parent].push_back(nodes[item->self]);
        // }

        // // write to file
        // struct Element
        // {
        //     pugi::xml_node xml;
        //     Node*          data;
        // };

        // pugi::xml_document  main;
        // Queue<Element>      queue;
        // queue.push({main, root()});

        // while (!queue.empty())
        // {
        //     auto data = queue.front().data;
        //     auto xml  = queue.front().xml;
        //     queue.pop();

        //     auto node = xml.append_child(data->name);

        //     for(auto&& child : parents[data->self]) 
        //         queue.push({node, child});
        // }

        //return main.save_file(pugi::as_utf8(_Path.wstring()).c_str());

        //return main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw);
    }

//protected:

    enum DirtyFlags
    {
        NONE,
        DIRTY_HIERARCHY
    };

    // info
    mutable std::vector<Node*> nodes;

    Node* append_node(Node* _Parent, const char* _Name, const char* _Value) const
    {
        auto item    = new Node(this);
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
    //---------------------------------------------------------------------------------------------------------------------
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
    //file read time
    Document doc;
    doc.read("C:/SDK/Qt_Projects/OpenGL/logs/NewFile.xml");
    doc.write("C:/SDK/Qt_Projects/OpenGL/logs/NewFile1.xml");
    for(auto&& node : doc.nodes) std::cout << node->self << "\t" << node->parent << "\t" << node->name << "\n";

    // auto start = Helpers::tic();
    // Document doc;
    // doc.read("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML.pwrct");
    // std::cout << "file read time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";
    // start = Helpers::tic();
    // doc.write("C:/SDK/Qt_Projects/OpenGL/logs/VeryLargeXML1.pwrct");    
    // std::cout << "file write time " << Helpers::elapsed<std::chrono::milliseconds>(start, Helpers::tic()) << " ms \n";

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