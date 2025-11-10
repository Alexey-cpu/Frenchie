#pragma once

namespace Frenchie
{
    namespace Core
    {
        template<typename Type>
        class DefaultAllocator
        {
        public:
            DefaultAllocator(){}
            ~DefaultAllocator(){}

            Type* allocate(int _Count)
            {
                return reinterpret_cast<Type*>(malloc(sizeof(Type) * _Count));
            }

            void deallocate(Type* _Memory)
            {
                if(_Memory != nullptr)
                    free(_Memory);
            }

            template<typename ... Args>
            Type* construct(Args ... _Args)
            {
                Type* memory = allocate(1);
                return new(memory) Type(_Args...);
            }

            void destroy(Type* _Object)
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