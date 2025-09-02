#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class ISerializer
            {
            public:
                ISerializer(){}
                virtual ~ISerializer(){}
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) = 0;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) = 0;
            };
        }
    }
}
