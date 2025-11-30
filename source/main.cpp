// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplication.hpp>

#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

// int main(int argc, char *argv[])
// {
//     Tree<std::string> tree;

//     auto child_1 = tree.construct_node(Node<std::string>(), "Child-1");
//     auto c11 = tree.construct_node(child_1, "Child-1-1");
//     tree.construct_node(c11, "Child-1-1-1");
//     tree.construct_node(child_1, "Child-1-2");
//     tree.construct_node(child_1, "Child-1-3");    

//     tree.sort();

//     for (int i = tree.Indexes[child_1.Index]; i < tree.Indexes[child_1.Index + 1]; i++)
//     {
//         std::cout << tree.Nodes[i].Data << "\n";
//     }

//     return 0;
// }

int main(int argc, char *argv[])
{
    Frenchie::Application::application()->push_layer<Frenchie::Application::Immedidate2DRendererTestLayer>();
    return Frenchie::Application::application()->execute();
}