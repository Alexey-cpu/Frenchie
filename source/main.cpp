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

template<typename T, int S = 512>
class Stack final
{
public:

    Stack()
    {
        container.resize(S);
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

template<typename T, int S = 512>
class Queue final
{
public:

    Queue()
    {
        container.resize(S);
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

class Document final
{
public:

    class NodePointer final
    {        
        NodePointer(const Document* _Document) : document(_Document){}
        ~NodePointer(){}

        mutable std::string name     = std::string();
        mutable std::string value    = std::string();
        mutable int         self     = 0;
        const Document*     document = nullptr;
        const NodePointer*      parent   = 0;

        friend class Document;
        friend class Iterator;
        friend class Node;
    };

    class Node final
    {
    public:

        class Iterator final
        {
        public:
            Iterator(const Node& _Node) : 
                m_Document(_Node.m_Pointer->document), 
                m_Index(_Node.self()){}
            
            ~Iterator(){}

            // access
            Node operator*() const 
            {
                if(m_Document == nullptr) 
                    return Node();

                const auto& hierarchy = m_Document->hierarchy();

                return hierarchy.entry(m_Index);
            }

            const Node* operator->() const
            {
                if(m_Document == nullptr) 
                    return nullptr;

                const auto& hierarchy = m_Document->hierarchy();

                return &hierarchy.entry(m_Index);
            }

            // prefix
            Iterator& operator++() 
            { 
                m_Index++; 
                return *this; 
            }

            Iterator& operator--() 
            { 
                m_Index--; 
                return *this; 
            }

            // postfix
            Iterator operator++(int) 
            { 
                Iterator tmp = *this; 
                ++(*this); 
                return tmp; 
            }

            Iterator operator--(int) 
            { 
                Iterator tmp = *this; 
                --(*this); 
                return tmp; 
            }

            // comparison
            friend bool operator==(const Iterator& a, const Iterator& b) 
            { 
                return a.m_Index == b.m_Index; 
            }
            
            friend bool operator!=(const Iterator& a, const Iterator& b) 
            { 
                return a.m_Index != b.m_Index; 
            }

            // arithmetics
            static int distance(const Iterator& _First, const Iterator& _Last)
            {
                return _Last.m_Index - _First.m_Index;
            }

        protected:
            const Document* m_Document;
            int             m_Index;
        };

        Node(const NodePointer* _Pointer = nullptr) : m_Pointer(_Pointer){}
        ~Node(){}

        const Iterator begin() const
        {
            return Iterator(*this);
        }

        const Iterator end() const
        {
            if(!valid()) 
                return Iterator(Node());

            const auto& hierarchy = 
                m_Pointer->document->hierarchy();

            return Iterator(hierarchy.items[hierarchy.pointers[self() + 1]]);
        }

        std::string& name() const
        {
            if(valid())
                return m_Pointer->name;

            Node::EMPTY_STRING = "";
            return Node::EMPTY_STRING;
        }

        std::string& value() const
        {
            if(valid())
                return m_Pointer->value;

            Node::EMPTY_STRING = "";
            return Node::EMPTY_STRING;
        }

        Node parent() const
        {
            return valid() ? Node(m_Pointer->parent) : Node();
        }

        int self() const
        {
            return valid() ? m_Pointer->self : 0; 
        }

        bool valid() const
        {
            return m_Pointer != nullptr && m_Pointer->document != nullptr;
        }

        bool empty() const
        {
            return Iterator::distance(begin(), end()) <= 0;
        }

        int size() const
        {
            return Iterator::distance(begin(), end());
        }

        void clear()
        {
            if(m_Pointer == nullptr) 
                return;

            delete m_Pointer;
            m_Pointer = nullptr;
        }

        Node append_child(const char* _Name, const char* _Value)
        {
            return valid() ? m_Pointer->document->append_child(_Name, _Value, *this) : Node();
        }

    private:
        inline static std::string EMPTY_STRING = "";

        const NodePointer* m_Pointer = nullptr;

        friend class Document;
    };

    struct TreeHierarchyMatrix final
    {
        TreeHierarchyMatrix(const std::vector<Node>& nodes = std::vector<Node>())
        {
            generate(nodes);
        }

        ~TreeHierarchyMatrix(){}

        bool is_dirty() const
        {
            return m_is_dirty || items.empty() || pointers.empty();
        }

        void set_dirty()
        {
            m_is_dirty = true;
        }

        void generate(const std::vector<Node>& nodes = std::vector<Node>())
        {
            if(nodes.empty()) 
                return;

            items.resize(nodes.size());
            pointers.resize(nodes.size() + 1);
            std::vector<int> workspace(nodes.size() + 1);

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
                if(!item.parent().valid()) 
                    continue;

                pointers[item.parent().self()]++;
                workspace[item.parent().self()]++;
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
                if(!nodes[i].parent().valid()) 
                    continue;

                int index    = workspace[nodes[i].parent().self()]++;
                items[index] = nodes[i];
            }

            m_is_dirty = false;
        }

        Node entry(int _Index) const
        {
            return _Index >= pointers.size() || pointers[_Index] >= items.size() || is_dirty() ? Node() : items[pointers[_Index]];
        }

        // info
        std::vector<Node> items      = std::vector<Node>();
        std::vector<int>  pointers   = std::vector<int>();
        bool              m_is_dirty = true;
    };

    Document(){}

    ~Document()
    {
        reset();
    }

    Node root() const
    {
        return nodes.empty() ? Node() : nodes[0];
    }

    Node append_child(const char* _Name, const char* _Value, Node& _Parent = Node()) const
    {
        // setup dirty flag
        matrix.set_dirty();

        // append child
        auto item    = new NodePointer(this);
        item->name   = _Name;
        item->value  = _Value;
        item->self   = std::max<int>((int)nodes.size(), 0);
        item->parent = _Parent.m_Pointer;
        nodes.push_back(item);
        return Node(item);
    }

    void remove_child(std::function<bool(Node&)> _Predicate, Node& _Parent = Node()) const
    {
        if(_Predicate == nullptr) 
            return;
        
        //auto& matrix = get_tree_matrix();

        // setup dirty flag
        matrix.set_dirty();
    }

    void reset()
    {
        // clear
        for(auto&& node : nodes) 
            node.clear();
        nodes.clear();

        // setup dirty flag
        matrix.set_dirty();
    }

    TreeHierarchyMatrix& hierarchy() const
    {
        if(matrix.is_dirty()) 
            matrix.generate(nodes);

        return matrix;
    }

protected:

    // info
    mutable std::vector<Node>   nodes;
    mutable TreeHierarchyMatrix matrix;
};

template<typename T>
class Format final
{
public:
    
    static bool read(Document& _Document, const std::filesystem::path& _Path)
    {
        return T::read(_Document, _Path);
    }

    static bool write(Document& _Document, const std::filesystem::path& _Path)
    {
        return T::write(_Document, _Path);
    }
};

template<bool Compact = false>
class XML final
{
public:

    // write a file
    struct Element
    {
        pugi::xml_node document;
        Document::Node data;
    };

    static bool read(Document& _Document, const std::filesystem::path& _Path)
    {
        // load file
        pugi::xml_document doc;
        auto status = doc.load_file(_Path.c_str()).status;

        if(status != pugi::xml_parse_status::status_ok)
        {
            status = doc.load_file(&pugi::as_utf8(_Path.wstring())[0]).status;

            if(status != pugi::xml_parse_status::status_ok) 
                return false;
        }

        if(doc.empty()) 
            return false;

        // clear self
        _Document.reset();

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
                        _Document.append_child(element.name(), element.value(), top.data)
                    }
                );
            }
        }

        return true;
    }

    static bool write(Document& _Document, const std::filesystem::path& _Path)
    {
        // compute matrix
        const auto& matrix = _Document.hierarchy();

        pugi::xml_document main;
        Queue<Element>     queue;
        queue.push({main, _Document.root()});

        while (!queue.empty())
        {
            auto data = queue.front().data;
            auto xml  = queue.front().document;
            queue.pop();

            auto node = xml.append_child(data.name());

            for (size_t i = matrix.pointers[data.self()]; i < matrix.pointers[data.self() + 1]; i++) 
                queue.push({node, matrix.items[i]});
        }

        return Compact ? 
                main.save_file(pugi::as_utf8(_Path.wstring()).c_str(), "\t", pugi::format_raw) : 
                main.save_file(pugi::as_utf8(_Path.wstring()).c_str());
    }
};

//------------------------------------------------------------------------------------------------

int main(int, char**)
{
    Document doc;

    auto root = doc.append_child("Root", "Zero");

    auto child = 
        root.append_child("Child-1", "Zero")
            .append_child("Child-1-1", "Value-1-1")
            .append_child("Child-1-2", "Value-1-2");

    child.append_child("Child-2", "Zero");
    child.append_child("Child-3", "Zero");

    for(auto&& item : root)
    {
        std::cout << item.name() << "\n";
    }

    std::cout << "root.empty() " << root.empty() << "\n";
    std::cout << "root.size() " << root.size() << "\n";

    Format<XML<false>>::write(doc,"C:/SDK/Qt_Projects/OpenGL/logs/TestFile.xml");
    Format<XML<false>>::read(doc, "C:/SDK/Qt_Projects/OpenGL/logs/TestFile.xml");
    Format<XML<false>>::write(doc,"C:/SDK/Qt_Projects/OpenGL/logs/TestFile1.xml");

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