#include <FrenchieCoreObject.hpp>
#include <iostream>

class Root : public Frenchie::Core::Object
{
public:
    Root(Frenchie::Core::Object* _Parent = nullptr) :  Frenchie::Core::Object("Root", _Parent){}
    virtual ~Root()
    {
        std::cout << "~Root() \n";
    }
};

class Object_1 : public Frenchie::Core::Object
{
public:
    Object_1(Frenchie::Core::Object* _Parent = nullptr) :  Frenchie::Core::Object("Object_1", _Parent){}
    virtual ~Object_1()
    {
        std::cout << "~Object_1() \n";
    }
};

class Object_2 : public Frenchie::Core::Object
{
public:
    Object_2(Frenchie::Core::Object* _Parent = nullptr) :  Frenchie::Core::Object("Object_2", _Parent){}
    virtual ~Object_2()
    {
        std::cout << "~Object_2() \n";
    }
};

class Object_3 : public Frenchie::Core::Object
{
public:
    Object_3(Frenchie::Core::Object* _Parent = nullptr) :  Frenchie::Core::Object("Object_3", _Parent){}
    virtual ~Object_3()
    {
        std::cout << "~Object_3() \n";
    }
};

int main(int, char**)
{
    Frenchie::Core::Object* root    = new Root();
    Frenchie::Core::Object* child_1 = new Object_1();
    Frenchie::Core::Object* child_2 = new Object_2();
    Frenchie::Core::Object* child_3 = new Object_3();

    child_1->set_parent(root);
    child_2->set_parent(child_1);
    child_3->set_parent(child_2);

    std::cout << "apply_to_children_recursive \n";

    root->apply_to_children_recursive([](Frenchie::Core::Object* _Object)
    {
        std::cout << _Object->get_name() << "\n";
    });

    std::cout << "find_child_recursive<T> \n";
    auto found = root->find_child_recursive<Object_3>();
    if(found != nullptr) 
        std::cout << found->get_name() << "\n";

    std::cout << "get_parent \n";
    std::cout << child_1->get_parent()->get_name() << "\n";
    std::cout << child_2->get_parent()->get_name() << "\n";
    std::cout << child_3->get_parent()->get_name() << "\n";

    std::cout << "get_parent \n";
    child_1->set_parent(child_3);
    if(child_1->get_parent())std::cout << "child_1->get_parent " << child_1->get_parent()->get_name() << "\n";
    if(child_2->get_parent())std::cout << "child_2->get_parent " << child_2->get_parent()->get_name() << "\n";
    if(child_3->get_parent())std::cout << "child_3->get_parent " << child_3->get_parent()->get_name() << "\n";

    //delete root;

    return 1;
}

// #include <FrenchieApplication.hpp>
// #include <FrenchieImGuiTestLayer.hpp>
// #include <FrenchieRenderingTestLayer.hpp>

// int main(int, char**)
// {
//     // setup logger
//     Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();

//     // setup application
//     auto application = Frenchie::Application::Application::instance();
//     application->set_window_size(glm::vec2(2048, 1024));
//     application->set_maximized(true);

//     // push application layers
//     application->push<Frenchie::Application::Test::RenderingTest>("RenderingTest");

//     return application->execute();
// }