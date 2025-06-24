#include <memory>

namespace Frenchie
{
    namespace Core
    {
        template<typename __type, typename ... __arguments>
        std::shared_ptr<__type> CreateShaderPointer(__arguments... _Parameters)
        {
            return std::make_shared<__type>(_Parameters ...);
        }

        template<typename __type, typename ... __arguments>
        std::unique_ptr<__type> CreateUniquePointer(__arguments... _Parameters)
        {
            return std::make_unique<__type>(_Parameters ...);
        }

        template<typename __type, typename ... __arguments>
        __type* CreateRawPointer(__arguments... _Parameters)
        {
            return new __type(_Parameters ...);
        }
    }
}