#pragma once

// STL
#include <functional>
#include <memory>
#include <string>
#include <mutex>
#include <map>

namespace Frenchie
{
    namespace Core
    {
        namespace Memory
        {
            template<typename Type>
            class Cache
            {
            public:

                template<typename ...Args>
                static std::shared_ptr<Type> request(const std::u32string& _Key, Args ... _Args)
                {
                    std::lock_guard<std::mutex> hold(m_Mutex);
                    auto sp = m_Cache[_Key].lock();
                    
                    if(!sp) 
                        m_Cache[_Key] = sp = std::make_shared<Type>(_Args...);
                    
                    return sp;
                }

            protected:
                inline static std::map<std::u32string, std::weak_ptr<Type>> m_Cache;
                inline static std::mutex                                    m_Mutex;
            };
        }
    }
};