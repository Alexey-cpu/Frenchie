#pragma once

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