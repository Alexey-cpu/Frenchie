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

//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------

#include "pugixml.hpp"

struct Property
{
    std::string Name;
    std::any    Value;

    template<typename Type> 
    Type get() const
    {
        try
        {
            return std::any_cast<Type>(Value)
        }
        catch(...)
        {
            return Type();
        } 
    }

    template<typename Type> 
    void set(const Type& _Value) const
    {
        Value = _Value;
    }

    std::string to_string() const
    {
        try
        {
            return std::to_string(std::any_cast<bool>(Value));
        }
        catch(...)
        {
        }

        try
        {
            return std::to_string(std::any_cast<float>(Value));
        }
        catch(...)
        {
        }

        try
        {
            return std::to_string(std::any_cast<double>(Value));
        }
        catch(...)
        {
        }

        try
        {
            return std::any_cast<std::string>(Value);
        }
        catch(...)
        {
        }

        return std::string();
    }

    // nested types
    struct TransparentComparator
    {
        using is_transparent = Property;

    public:

        bool operator()(const Property& _A, const Property& _B) const
        {
            return  _A.Name < _B.Name;
        }
    };
};


class Node
{
public:
    Node(const std::string& _Name) : 
        m_Name(_Name){}
    
    virtual ~Node(){}

    std::string get_name() const
    {
        return m_Name;
    }

    std::vector<Node*> get_children() const
    {
        if(m_Children.empty()) 
            return std::vector<Node*>();
        
        std::vector<Node*> children;

        for(auto&& child : m_Children) 
            children.push_back(child.get());

        return children;
    }

    Node* append_child(const std::string& _Name)
    {
        m_Children.push_back(std::make_unique<Node>(_Name));
        auto& child = m_Children.back();
        child->m_Parent = this;
        return child.get();
    }

    template<bool _Recursive = true>
    void apply_to_children(const std::function<void(Node* _Object)>& _Callback) const
    {
        if(_Callback == nullptr) 
            return;

        for(auto&& child : m_Children) 
        {
            if(child == nullptr) 
                continue;

            _Callback(child.get());

            if(_Recursive)
                child->apply_to_children(_Callback);
        }
    }

    std::set<Property, Property::TransparentComparator> Properties = 
        std::set<Property, Property::TransparentComparator>();

protected:

    Node*       m_Parent = nullptr;
    std::string m_Name   = std::string();

    std::vector<std::unique_ptr<Node>> m_Children = 
        std::vector<std::unique_ptr<Node>>();
};

template<typename T>
class Format
{
public:
    Format(){}
    ~Format(){}

    std::shared_ptr<Node> read(const std::filesystem::path& _Path)
    {
        return T::read(_Path);
    }

    bool write(std::shared_ptr<Node>& _Node, const std::filesystem::path& _Path)
    {
        return T::write(_Node);
    }
};

class XML : public Format<XML>
{
public:
    XML(){}
    ~XML(){}

    static std::shared_ptr<Node> read(const std::filesystem::path& _Path)
    {
        // try to parse project from XML file
        pugi::xml_document doc;

        auto status = doc.load_file(_Path.c_str()).status;

        if(status != pugi::xml_parse_status::status_ok)
        {
            if(doc.load_file(&_Path.wstring()[0]).status != pugi::xml_parse_status::status_ok) 
                return std::make_shared<Node>("EMPTY");
        }

        if(doc.empty()) 
            return std::make_shared<Node>("EMPTY");
        

        struct Element
        {
            pugi::xml_node node   = pugi::xml_node();
            Node*          object = nullptr;
        };

        std::shared_ptr<Node> root = std::make_shared<Node>(doc.first_child().name());
        std::queue<Element>   queue = std::queue<Element>();
        queue.push({doc.first_child(), root.get()});

        std::cout << "doc.name() " << doc.first_child().name() << "\n";

        while(!queue.empty())
        {
            auto top = queue.front();
            queue.pop();

            if(top.node.attribute("Type").as_string() == "Object")
            {
                auto child = top.object->append_child(top.node.name());

                for(auto& element : top.node)
                {
                    if(top.node.attribute("Type").as_string() == "bool") 
                        child->Properties.insert({element.name(), element.text().as_bool()});
                    else if(top.node.attribute("Type").as_string() == "double") 
                        child->Properties.insert({element.name(), element.text().as_double()});
                    else if(top.node.attribute("Type").as_string() == "float") 
                        child->Properties.insert({element.name(), element.text().as_float()});
                    else if(top.node.attribute("Type").as_string() == "int") 
                        child->Properties.insert({element.name(), element.text().as_int()});
                    else if(top.node.attribute("Type").as_string() == "Object") 
                        queue.push({element, child});
                }
            }
        }

        return root;
    }
};

int main(int, char**)
{
    auto node = XML().read("C:/SDK/Qt_Projects/OpenGL/logs/XML.xml");

    std::cout << "root " << node->get_name() << "\n";

    node->apply_to_children([](Node* _Object)
    {
        std::cout << _Object->get_name() << "\n";

        for(auto&& property : _Object->Properties)
        {
            std::cout << property.Name << "\t" << property.to_string() << "\n";
        }
    }
    );

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