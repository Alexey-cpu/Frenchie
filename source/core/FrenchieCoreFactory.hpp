#pragma once

// Custom
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreHelpers.hpp>

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
                        fmt::format("{} could not invoke factory method for {}", STRINGIFY(Frenchie::Core::Factory), _Name));
                    return nullptr;
                }
            }

            // nested types
            template<typename Base>
            class Creator
            {
            public:

                template<typename Type, typename ... FactoryArgs>
                class Registry : public Base
                {
                public:
                    template<typename ... Args>
                    Registry(Args ... _Args) : Base(_Args...){}
                    virtual ~Registry(){}

                    using TRegistryType = Registry<Type, FactoryArgs ...>;
                    using TReturnType   = std::unique_ptr<Base>;

                    inline static bool registerFactory()
                    {
                        if(TRegistryType::m_Registered) 
                            return true;

                        Factory::registry()[Type::factory_id()] = 
                            std::function<TReturnType(FactoryArgs ...)>(
                                [](FactoryArgs ... _Args)->TReturnType
                                {
                                    return std::make_unique<Type>(std::forward<FactoryArgs>(_Args)...);
                                }
                            );

                        return true;
                    }

                protected:
                    inline static bool m_Registered = 
                        TRegistryType::registerFactory();
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
