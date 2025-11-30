// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplication.hpp>

#include <FrenchieApplicationSandBoxImmediate2DRendererTestLayer.hpp>

int main(int argc, char *argv[])
{
    Frenchie::Application::application()->push_layer<Frenchie::Application::Immedidate2DRendererTestLayer>();
    return Frenchie::Application::application()->execute();
}

// #include <FrenchieCoreTime.hpp>
// #include <FrenchieCoreMemoryChunkAllocator.hpp>
// #include <FrenchieCoreContainersObjectList.hpp>

// template<typename Type> struct Tree;

// template<typename Type>
// struct Node
// {
//     int               Parent{-1};
//     int               Index {-1};
//     Type              Data  {Type()};
//     const Tree<Type>* Tree  {nullptr};
// };

// template<typename Type>
// struct Tree
// {
//     std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
//     std::vector<int>        Indexes{std::vector<int>()};
//     std::vector<int>        Entries{std::vector<int>()};
//     bool                    Dirty  {true};

//     template<typename ... Args>
//     Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
//     {
//         Node<Type> node;
//         node.Parent = _Parent.Index;
//         node.Index  = (int)Nodes.size();
//         node.Data   = Type(_Args ...);
//         node.Tree   = this;
//         Nodes.push_back(node);
//         return node;
//     }

//     void sort()
//     {
//         std::vector<Node<Type>> nodes(Nodes.size());
//         std::vector<int> workspace(Nodes.size()+1);

//         Indexes.resize(Nodes.size() + 1);
//         Entries.resize(Nodes.size());

//         for(int i = 0; i < Entries.size(); i++)
//         {
//             Entries[i] = 0;
//             Indexes[i] = 0;
//         }

//         // count items
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             if(Nodes[i].Parent < 0) continue;
//             ++Entries[Nodes[i].Parent];
//         }

//         // cumulative sum
//         int sum = 0;
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             Indexes  [i] = sum;
//             workspace[i] = sum;
//             sum += Entries[i];
//         }
//         Indexes[Nodes.size()] = sum;

//         for(int i = 0; i < Nodes.size(); i++ )
//         {
//             if(Nodes[i].Parent < 0) continue;
//             nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
//         }

//         Nodes = nodes;
//     }

//     void clear()
//     {
//         Nodes.clear();
//     }
// };

// int main(int argc, char *argv[])
// {
//     Tree<int> tree;

//     int N = (int)1e6;

//     for(int frame = 0; frame < 3; frame++)
//     {
//         auto then = Frenchie::Core::Time::tic();
//         for (int i = 0; i < N; i++) tree.construct_node(Node<int>(), 1);
//         std::cout << "creating " << N << " has taken " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms\n";
        
//         then = Frenchie::Core::Time::tic();
//         tree.clear();
//         std::cout << "removing " << N << " has taken " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms\n";
//     }

//     return 0;
// }

// class StringNode : public Frenchie::Core::Containers::ObjectTreeNode<StringNode, Frenchie::Core::Memory::MemoryChunkAllocator<StringNode, true>>
// {
// public:

//     StringNode(const int& _Value = -1) : Value(_Value){}

//     int Value;

// };

// class StringTree : public Frenchie::Core::Containers::ObjectTreeRoot<StringNode, Frenchie::Core::Memory::MemoryChunkAllocator<StringNode, true>>
// {
// public:
// };

// void show_node(const StringNode* _Node, const std::string _Delimiter = "")
// {
//     std::cout << _Delimiter << _Node->Value << "\n";

//     for(auto&& child : *_Node)
//         show_node(child, _Delimiter + '\t');
// }

// void show_tree(const StringTree* _Tree)
// {
//     for(auto&& child : *_Tree)
//     {
//         show_node(child);
//     }
// }

// int main(int argc, char *argv[])
// {
//     StringTree tree;

//     // auto child1 = tree.insert_after(tree.last(), "child-1");
//     // auto child11 = (*child1)->insert_after((*child1)->last(), "child-1-1");
//     // auto child12 = (*child1)->insert_after((*child1)->last(), "child-1-2");
//     // auto child13 = (*child1)->insert_after((*child1)->last(), "child-1-3");
//     // auto child14 = (*child1)->insert_after((*child1)->last(), "child-1-4");
//     // auto child2 = tree.insert_after(tree.last(), "child-2");

//     //show_tree(&tree);

//     int N = (int)1e6;

//     for(int frame = 0; frame < 3; frame++)
//     {
//         auto then = Frenchie::Core::Time::tic();
//         for (int i = 0; i < N; i++) tree.insert_after(tree.last());
//         std::cout << "creating " << N << " has taken " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms\n";
        
//         then = Frenchie::Core::Time::tic();
//         tree.clear();
//         std::cout << "removing " << N << " has taken " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms\n";
//         std::cout << "Allocator total memory consumption " << tree.Allocator->get_total_memory_size() << "\n";
//         std::cout << "Allocator free memory amout        " << tree.Allocator->get_free_memory_amount() << "\n";
//         std::cout << "Allocator busy memory amout        " << tree.Allocator->get_busy_memory_amount() << "\n";
//         std::cout << "Allocator total chunks number      " << tree.Allocator->get_total_chunks_number() << "\n";
//     }

//     return 0;
// }