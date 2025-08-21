#pragma once

// Custom
#include <FrenchieCoreLogger.hpp>

// STL
#include <memory>
#include <map>
#include <any>

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
                    Logger::instance()->error(STRINGIFY(Frenchie::Core::Factory));
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
                    Logger::instance()->error(STRINGIFY(Frenchie::Core::Factory));
                    Logger::instance()->error("------------------------------------------------------------------------------");
                    Logger::instance()->error(fmt::format("Factory method for {} does not exist", _Name));
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

                    Registry(const std::string& _Name) : Base(_Name)
                    {
                        if(!m_Registered) 
                            m_Registered = registerFactory(_Name);
                    }

                    virtual ~Registry(){}

                protected:

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
    }
};
