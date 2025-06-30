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

#define STRINGIFY(_VALUE) #_VALUE

namespace Frenchie
{
    namespace Core
    {
        class Factory
        {
        public:

            using TRegistry = std::map<std::string, std::any>;

            template<typename Type, typename ...Args>
            static std::unique_ptr<Type> create(std::string _Name, Args ... _Args)
            {
                using TReturnType = std::unique_ptr<Type>;

                auto it = registry().find(_Name);

                if(it == registry().end())
                {
                    Logger::instance()->error("------------------------------------------------------------------------------");
                    Logger::instance()->error("FRENCHIE::CORE::FACTORY::ERROR");
                    Logger::instance()->error("------------------------------------------------------------------------------");
                    Logger::instance()->error(fmt::format("Factory method for {} does not exist", _Name));
                    return nullptr;
                }

                try
                {
                    auto factoryMethod = 
                        std::any_cast<std::function<TReturnType(Args...)>>((*it).second);

                    return factoryMethod(std::forward<Args>(_Args)...);
                }
                catch(...)
                {
                    Logger::instance()->error("------------------------------------------------------------------------------");
                    Logger::instance()->error("FRENCHIE::CORE::FACTORY::ERROR");
                    Logger::instance()->error("------------------------------------------------------------------------------");
                    Logger::instance()->error(fmt::format("Factory method for {} does not exist 1", _Name));
                    return nullptr;
                }
            }

            // nested types
            template<typename Base>
            class Creator
            {
            public:
                using TReturnType = std::unique_ptr<Base>;

                template<typename Type>
                class Registry : public Base
                {
                public:

                    Registry(const std::string& _Name) : m_Name(_Name)
                    {
                        if(!m_Registered) 
                            m_Registered = registerFactory(_Name);
                    }

                    virtual ~Registry(){}

                    virtual std::string get_name() const override
                    {
                        return m_Name;
                    }

                protected:
                    std::string m_Name;

                    inline static bool m_Registered = false;

                    template<typename ...Args>
                    static bool registerFactory(const std::string& _Name)
                    {
                        Factory::registry()[_Name] = std::function<TReturnType(Args...)>(
                            [](Args... _Args)
                            {
                                return Type::create(std::forward<Args>(_Args)...);
                            }
                        );

                        return true;
                    }
                };
            };

            static TRegistry& registry()
            {
                static TRegistry registry;
                return registry;
            }
        };

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
    }
};
