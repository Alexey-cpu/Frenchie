#pragma once

#include <FrenchieApplicationThreadQueueLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        class LocalThreadQueue final
        {
        public:
            LocalThreadQueue();
            ~LocalThreadQueue();

            Frenchie::Core::Reference<Frenchie::Application::ThreadQueue> instance();

        protected:

            Frenchie::Core::Reference<Frenchie::Application::ThreadQueue> m_ThreadQueue;
        };
    }
}