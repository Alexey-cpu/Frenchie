#pragma

#include <FrenchieCoreObject.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class GameObject : public Frenchie::Core::Object
        {
        public:
            GameObject(const std::string _Name);
            virtual ~GameObject(){}
        };
    }
};