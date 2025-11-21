#pragma once

// STL
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        namespace Containers
        {
            template<typename Type, int Size = 512>
            class RingBuffer final
            {
            public:

                RingBuffer()
                {
                    m_Buffer.resize(Size);
                }

                ~RingBuffer(){}

                int get_position() const
                {
                    return m_Position;
                }

                void set_position(const int& _Value)
                {
                    m_Position = std::min<int>(std::max<int>(_Value, 0) % Size, m_HighestPosition);
                }

                void push(const Type& _Value)
                {
                    m_Buffer[m_Position++ % Size] = _Value;

                    m_HighestPosition =
                        m_Position <= 0 ? 0 : std::max<int>(m_Position, m_HighestPosition);
                }

                Type& at(const int& _Index)
                {
                    int index = m_Position - std::max<int>(_Index, 0);

                    if(index < 0)
                        index += Size;

                    return m_Buffer[index];
                }

            protected:
                int               m_HighestPosition = 0;
                int               m_Position        = 0;
                std::vector<Type> m_Buffer          = std::vector<Type>();
            };
        }
    }
}