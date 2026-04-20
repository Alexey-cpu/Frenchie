#pragma once

// STL
#include <vector>

/*! \defgroup <Core> (Core)
 *  @brief The module contains core utility functions and classes.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Core
    {
        /*! \defgroup <RingBuffer> (RingBuffer)
        *  @ingroup Core
        *  @brief The module contains core utility class that implementes static compile time ring buffer.
        * @{
        */

        /**
         *  @brief represents tatic compile time ring buffer.
         *  @class RingBuffer
         *  @tparam [Type] type of stored values
         *  @tparam [Size] size of ring buffer
         */
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

            /*!
             * @brief returns size of this ring buffer
             * 
             * @return returns size of this ring buffer 
             */
            int size() const
            {
                return Size;
            }

            /**
             * @brief pushes a value inside this ring buffer
             * 
             * @param [_Value] value to push into this ring buffer 
             */
            void push(const Type& _Value)
            {
                m_Buffer[m_Position] = _Value;
                if(++m_Position >= Size)
                    m_Position = 0;
            }

            /**
             * @brief returns value located within this ring buffer at predefined index
             * 
             * @param [_Index] index of a value
             */
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

        /*! @} */
    }
}