#pragma once

namespace Frenchie
{
    namespace Core
    {
        template<typename T>
        class Singleton
        {
        public:

            static T* instance()
            {
                static T enity;
                return &enity;
            }

            Singleton(const Singleton &) = delete;
            Singleton & operator = (const Singleton &) = delete;

        private:
            Singleton(){}
            ~Singleton(){}
        };
    }
}