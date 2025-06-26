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

                    Type* instance = Frenchie::Core::create_raw_pointer<Type>(_Args...);
                    m_Instances[typeIndex] = instance;
                    return instance;
                }

                void apply_to_all_instances(const std::function<void(std::any& _Instance)>& _Function)
                {
                    for (auto&& instance : m_Instances) 
                        _Function(instance.second);
                }

            protected:
                std::map<std::type_index, std::any> m_Instances;
            };
        }

        typedef Frenchie::Core::Singleton<Frenchie::Core::Factory::FlyweightFactory> FlyweightFactory;
    }
};
