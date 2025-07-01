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

        template<typename Type>
        class Cache
        {
        public:

            template<typename ...Args>
            static std::shared_ptr<Type> request(const std::string& _Key, Args ... _Args)
            {
                std::lock_guard<std::mutex> hold(m_Mutex);
                auto sp = m_Cache[_Key].lock();
                
                if(!sp) 
                    m_Cache[_Key] = sp = std::make_shared<Type>(_Args...);
                
                return sp;
            }

        protected:
            inline static std::map<std::string, std::weak_ptr<Type>> m_Cache;
            inline static std::mutex m_Mutex;
        };
    }
};
