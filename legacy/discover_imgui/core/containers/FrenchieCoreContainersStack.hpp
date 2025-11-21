#pragma once

// STL
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        namespace Containers
        {
            template<typename Type, int Size = 512>
            class Stack final
            {
            public:

                Stack()
                {
                    container.resize(Size);
                }

                std::vector<Type> container;

                void push(const Type& _Value)
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

                Type& top()
                {
                    return container[current];
                }

                bool empty() const
                {
                    return current < 0;
                }

                int current = -1;
                int next    = 0;
                int growth  = 2;
            };
        }
    }
}