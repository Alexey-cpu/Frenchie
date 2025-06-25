#pragma once

// Custom
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <typeindex>
#include <map>
#include <any>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        namespace Factory
        {
            class FlyweightFactory
            {
            public:
                FlyweightFactory(){}
                virtual ~FlyweightFactory(){}

                template<typename Type, typename ...Arguments> 
                Type* Create(Arguments ... _Args)
                {
                    std::type_index typeIndex = std::type_index(typeid(Type));

                    if(m_Instances.find(typeIndex) != m_Instances.end()) 
                        return std::any_cast<Type*>(m_Instances[typeIndex]);

                    Type* instance = Frenchie::Core::CreateRawPointer<Type>(_Args...);
                    m_Instances[typeIndex] = instance;
                    return instance;
                }

            protected:
                std::map<std::type_index, std::any> m_Instances;
            };
        }

        typedef Frenchie::Core::Singleton<Frenchie::Core::Factory::FlyweightFactory> FlyweightFactory;
    }
};
