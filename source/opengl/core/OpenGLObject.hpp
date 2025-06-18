#pragma once

// STL
#include <memory>

namespace OpenGL
{
    class Object
    {
    public:
        
        Object(unsigned int _Adress) : 
            m_Adress(_Adress){}
        
        virtual ~Object(){}

        const unsigned int& get_adress() const
        {
            return m_Adress;
        }

    private:

        unsigned int m_Adress;
    };

    template<typename __type, typename ... __parameters>
    std::shared_ptr<__type> Create(__parameters... _Parameters)
    {
        return std::make_shared<__type>(_Parameters ...);
    }
};