#pragma once

// Custom
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <typeindex>
#include <string>
#include <map>
#include <any>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        namespace Factory
        {
            class Flyweight
            {
            public:
                Flyweight(){}
                virtual ~Flyweight(){}

                template<typename Type, typename ...Arguments>
                Type* request(std::string _Key, Arguments ... _Args)
                {
                    const auto  typeIndex = std::type_index(typeid(Type));
                    auto& resources = m_Resources[typeIndex];

                    if(resources.find(_Key) != resources.end()) 
                        return std::any_cast<Type*>(resources[_Key]);

                    Type* instance = Frenchie::Core::create_raw_pointer<Type>(_Args...);
                    resources[_Key] = instance;
                    return instance;
                }

                template<typename Type>
                void apply_to_all_instances(const std::function<void(Type* _Instance)>& _Function)
                {
                    if(_Function == nullptr) 
                        return;

                    const auto  typeIndex = std::type_index(typeid(Type));
                    const auto& resources = m_Resources[typeIndex];

                    for (auto&& resource : resources)
                    {
                        try
                        {
                            _Function(std::any_cast<Type*>(resource.second));
                        }
                        catch(...)
                        {
                        }
                    }
                }

            protected:

                std::map<
                    std::type_index, 
                    std::map<
                        std::string, 
                        std::any
                        >> m_Resources;
            };
        }

        typedef Frenchie::Core::Singleton<Frenchie::Core::Factory::Flyweight> Flyweight;
    }
};
