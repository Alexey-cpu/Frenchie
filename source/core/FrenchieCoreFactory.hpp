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
                    Logger::instance()->error(
                        fmt::format("{}: factory method for {} does not exist", STRINGIFY(Frenchie::Core::Factory), _Name));
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

                    Logger::instance()->error(
                        fmt::format("{} factory method for {} does not exist", STRINGIFY(Frenchie::Core::Factory), _Name));
                    return nullptr;
                }
            }

            // nested types
            template<typename Base>
            class Creator
            {
            public:

                template<typename Type>
                class Registry : public Base
                {
                public:
                    template<typename ... Args>
                    Registry(Args ... _Args) : Base(_Args...){}
                    virtual ~Registry(){}

                protected:
                    using TReturnType = std::unique_ptr<Base>;

                    static bool registerFactory()
                    {
                        Factory::registry()[Type::factory_id()] = 
                            std::function<TReturnType()>([](){return Type::create();});

                        return true;
                    }

                    inline static bool m_Registered = Registry<Type>::registerFactory();
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
