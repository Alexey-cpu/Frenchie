#pragma once

#include <FrenchieObject.hpp>

namespace Frenchie
{
    namespace Core
    {
        class Node;
        class Edge;
        class Graph;

        class Node : public Object
        {
        public:
            Node(){}
            virtual ~Node(){}
        protected:
            std::list<Edge*> m_AdjacentEdges = std::list<Edge*>();
        };

        class Edge : public Object
        {
        public:
            Edge(){}
            virtual ~Edge(){}
        
        protected:
            Node* m_Source = nullptr;
            Node* m_Target = nullptr;

        private:
            std::list<Edge*>::iterator m_SourceSelfIterator;
            std::list<Edge*>::iterator m_TargetSelfIterator;
        };

        class Graph : public Object
        {
        public:
            Graph(){}
            virtual ~Graph(){}
        protected:
        };
    }
}