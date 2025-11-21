#pragma once

namespace Frenchie
{
    namespace Core
    {
        class NonCopyable 
        {
        protected:
            NonCopyable() = default;
            virtual ~NonCopyable() = default;

        public:
            NonCopyable(const NonCopyable &) = delete;
            NonCopyable(NonCopyable &&) noexcept = default;
            NonCopyable &operator=(const NonCopyable &) = delete;
            NonCopyable &operator=(NonCopyable &&) noexcept = default;
        };
    }
}