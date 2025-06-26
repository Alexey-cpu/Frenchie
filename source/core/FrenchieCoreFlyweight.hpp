#pragma once

// Custom
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreSingleton.hpp>

// STL
#include <memory>
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
                    const auto typeIndex = std::type_index(typeid(Type));
                    auto&      resources = m_Resources[typeIndex];
                    auto       iterator  = resources.find(_Key);

                    if(iterator != resources.end()) 
                        return std::dynamic_pointer_cast<Wrapper<Type>>(iterator->second)->get_data();

                    auto instance = create_raw_pointer<Type>(_Args...);
                    resources[_Key] = std::make_shared<Wrapper<Type>>(instance);
                    return instance;
                }

                template<typename Type>
                void apply_to_all_instances(const std::function<void(Type* _Instance)>& _Function)
                {
                    if(_Function == nullptr) 
                        return;

                    const auto  typeIndex = std::type_index(typeid(Type));
                    const auto& resources = m_Resources[typeIndex];

                    for(auto&& resource : resources)
                    {
                        try
                        {
                            _Function(std::dynamic_pointer_cast<Wrapper<Type>>(resource.second)->get_data());
                        }
                        catch(...)
                        {
                        }
                    }
                }

            protected:

                class Data
                {
                public:
                    Data(){}
                    virtual ~Data(){}
                };

                template<typename __type>
                class Wrapper : public Data
                {
                public:
                    
                    Wrapper(__type* _Data) : m_Data(_Data){}
                    
                    virtual ~Wrapper()
                    {
                        if(m_Data != nullptr) 
                            delete m_Data;
                    }
                    
                    __type* get_data() const
                    {
                        return m_Data;
                    }
                    
                protected:
                    __type* m_Data = nullptr;
                };

                std::map<
                    std::type_index,          // data type
                    std::map<
                        std::string,          // data key
                        std::shared_ptr<Data> // data container
                        >> m_Resources;
            };
        }

        typedef Frenchie::Core::Singleton<Frenchie::Core::Factory::Flyweight> Flyweight;
    }
};
