#pragma once

#include <FrenchieCoreSerialization.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XML : public Format
            {
            public:
                XML();
                virtual ~XML();
                virtual std::shared_ptr<Node> read(const std::filesystem::path& _Path) override;
                virtual bool write(const Reference<Node>& _Node, const std::filesystem::path& _Path) override;
            };
        }
    }
}