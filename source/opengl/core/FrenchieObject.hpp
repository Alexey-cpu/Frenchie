#pragma once

// STL
#include <memory>

namespace Frenchie
{
    class Object
    {
    public:
        
        Object(unsigned int _ID) : m_ID(_ID){}
        virtual ~Object(){}

        const unsigned int& get_id() const
        {
            return m_ID;
        }

    private:
        unsigned int m_ID;
    };

    template<typename __type, typename ... __arguments>
    std::shared_ptr<__type> Create(__arguments... _Parameters)
    {
        return std::make_shared<__type>(_Parameters ...);
    }
}