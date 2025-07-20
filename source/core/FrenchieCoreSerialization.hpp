#pragma once

#include <FrenchieCoreReference.hpp>
#include <FrenchieCoreHelpers.hpp>

// PUGIXML
#include "pugixml.hpp"

// STL
#include <filesystem>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <queue>
#include <any>
#include <set>

namespace Frenchie
{
    namespace Core
    {
        class DocumentNode
        {
        public:

            struct Property
            {
                Property(const std::string& _Name, const std::any& _Value) : 
                    m_Name(_Name),
                    m_Value(_Value){}

                std::string get_name() const
                {
                    return m_Name;
                }

                template<typename Type> 
                Type get() const
                {
                    try
                    {
                        return std::any_cast<Type>(m_Value);
                    }
                    catch(...)
                    {
                        return Type();
                    } 
                }

                const type_info& get_type() const
                {
                    return m_Value.type();
                }

                template<typename Type> 
                void set(const Type& _Value) const
                {
                    m_Value = _Value;
                }

            private:

                const   std::string m_Name;
                mutable std::any    m_Value;
            };

            struct PropertyComparator
            {
                using is_transparent = Property;

            public:

                bool operator()(const Property& _A, const Property& _B) const
                {
                    return  _A.get_name() < _B.get_name();
                }
            };

            DocumentNode(const std::string& _Name);
            virtual ~DocumentNode();

            std::string get_name() const;
            std::vector<Reference<DocumentNode>> get_children() const;

            void set_name(const std::string& _Name);

            Reference<DocumentNode> append_child(const std::string& _Name);

            std::set<Property, PropertyComparator> Properties = 
                std::set<Property, PropertyComparator>();

        protected:

            std::string m_Name = std::string();

            std::vector<std::shared_ptr<DocumentNode>> m_Children = 
                std::vector<std::shared_ptr<DocumentNode>>();
        };

        class IDocumentWriter
        {
        public:
            IDocumentWriter(){}
            virtual ~IDocumentWriter(){}
            virtual std::shared_ptr<DocumentNode> read(const std::filesystem::path& _Path) = 0;
            virtual bool write(const Reference<DocumentNode>& _Node, const std::filesystem::path& _Path) = 0;
        };

        class XMLDocumentWriter : public IDocumentWriter
        {
        public:
            XMLDocumentWriter();
            virtual ~XMLDocumentWriter();
            std::shared_ptr<DocumentNode> read(const std::filesystem::path& _Path);
            bool write(const Reference<DocumentNode>& _Node, const std::filesystem::path& _Path);

        protected:
            void write(const Reference<DocumentNode>& _Node, pugi::xml_node& _Document);
        };
    }
}