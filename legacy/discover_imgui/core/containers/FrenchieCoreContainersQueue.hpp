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
            class Queue final
            {
            public:

                Queue()
                {
                    container.resize(Size);
                }

                std::vector<Type> container;

                void push(const Type& _Value)
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

                Type& front()
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
        }
    }
}