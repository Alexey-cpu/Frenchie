#pragma once

// Custom
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreSingleton.hpp>
#include <FrenchieCoreLogger.hpp>

// STL
#include <memory>
#include <typeindex>
#include <functional>
#include <string>
#include <map>
#include <any>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        template<typename Key>
        class Flyweight
        {
        public:

            Flyweight(){}
            virtual ~Flyweight()
            {
                const auto keyType = std::type_index(typeid(Key));

                Logger::instance()->info("------------------------------------------------------------------------------");
                Logger::instance()->info("FRENCHIE::CORE::FLYWEIGHT::RELEASE");
                Logger::instance()->info("------------------------------------------------------------------------------");
                Logger::instance()->info(fmt::format("[Key]: {}", keyType.name()));

                for(auto&& resources : m_Resources)
                {
                    Logger::instance()->info(fmt::format("[Resource]: {} {}", resources.first.name(), resources.second.size()));

                    for(auto&& resource : resources.second)
                    {
                        if(resource.second != nullptr) 
                            delete resource.second;
                    }
                }
            }

            template<typename Type, typename ...Arguments>
            Type* request(Key _Key, Arguments ... _Args)
            {
                const auto typeIndex = std::type_index(typeid(Type));
                auto&      resources = m_Resources[typeIndex];
                auto       iterator  = resources.find(_Key);

                if(iterator != resources.end()) 
                    return dynamic_cast<Wrapper<Type>*>(iterator->second)->get_data();

                auto instance   = new Type(_Args...);
                resources[_Key] = new Wrapper<Type>(instance);
                return instance;
            }

            template<typename Type>
            void apply_function_to_instances(const std::function<void(Type* _Instance)>& _Function)
            {
                if(_Function == nullptr) 
                    return;

                const auto  typeIndex = std::type_index(typeid(Type));
                const auto& resources = m_Resources[typeIndex];

                for(auto&& resource : resources)
                {
                    Wrapper<Type>* object = 
                        dynamic_cast<Wrapper<Type>*>(resource.second);

                    if(object != nullptr)
                        _Function(object->get_data());
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
                std::type_index,   // data type
                std::map<
                    Key,           // data key
                    Data*          // data container
                    >> m_Resources;
        };

        template<typename Base, class... Args>
        class Factory {
        public:
            using TCreateReturn = std::unique_ptr<Base>;
            using TCreateMethod = std::function<TCreateReturn(Args...)>;
            using TRegistryMap  = std::unordered_map<std::string, TCreateMethod>;

            virtual ~Factory() = default;

            static TCreateReturn Create(const std::string &name, Args &&... args) 
            {
                auto it = Registry().find(name);
                
                if (it == Registry().end()) 
                {
                    //Log::Error("Failed to create ", std::quoted(name), " from factory\n");
                    return nullptr;
                }

                return it->second(std::forward<Args>(args)...);
            }

            static TRegistryMap &Registry() 
            {
                static TRegistryMap impl;
                return impl;
            }

            template<typename T>
            class Registrar : public Base 
            {
            protected:
                
                static bool Register(const std::string &name) 
                {
                    Factory::Registry()[name] = [](Args... args)->TCreateReturn 
                    {
                        return std::make_unique<T>(std::forward<Args>(args)...);
                    };

                    return true;
                }
            };
        };
    }
};
