#pragma once

// STL
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        template<typename Type, int Size = 512>
        class RingBuffer final
        {
        public:

            RingBuffer()
            {
                m_Buffer.resize(Size);
            }

            RingBuffer(const Type& _Value)
            {
                m_Buffer.resize(Size);

                for (int i = 0; i < m_Buffer.size(); i++)
                    m_Buffer[i] = _Value;
            }

            ~RingBuffer(){}

            int size() const
            {
                return Size;
            }

            void push(const Type& _Value)
            {
                m_Buffer[m_Position] = _Value;
                if(++m_Position >= Size)
                    m_Position = 0;
            }

            Type& at(const int& _Index)
            {
                int index = m_Position - 1 - _Index;
                while(index < 0) index += Size;
                return m_Buffer[index];
            }

        protected:
            int               m_Position = 0;
            std::vector<Type> m_Buffer   = std::vector<Type>();
        };
    }
}