#pragma once

#include <FrenchieCoreSerializationNode.hpp>

namespace Frenchie
{
    namespace Core
    {
        namespace Serialization
        {
            class XML
            {
            public:
                XML();
                ~XML();
                static std::shared_ptr<Node> read(const std::filesystem::path& _Path);
                static bool write(Node* _Node, const std::filesystem::path& _Path);

            protected:
            
                struct Element
                {
                    pugi::xml_node node   = pugi::xml_node();
                    Node*          object = nullptr;
                };
            };
        }
    }
}