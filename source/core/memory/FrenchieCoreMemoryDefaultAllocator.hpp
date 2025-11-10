#pragma once

namespace Frenchie
{
    namespace Core
    {
        namespace Memory
        {
            template<typename Type>
            class DefaultAllocator
            {
            public:
                DefaultAllocator(){}
                ~DefaultAllocator(){}

                Type* allocate(int _Count) const
                {
                    return reinterpret_cast<Type*>(malloc(sizeof(Type) * _Count));
                }

                void deallocate(Type* _Memory) const
                {
                    if(_Memory != nullptr)
                        free(_Memory);
                }

                template<typename ... Args>
                Type* construct(Args ... _Args) const
                {
                    Type* memory = allocate(1);
                    return new(memory) Type(_Args...);
                }

                void destroy(Type* _Object) const
                {
                    if(_Object != nullptr)
                    {
                        _Object->~Type();
                        deallocate(_Object);
                    }
                }
            };
        }
    }
}