#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <vector>

namespace Frenchie
{
    namespace Core
    {
        class MeshGenerator2D final
        {
        public:
            MeshGenerator2D()  = default;
            ~MeshGenerator2D() = default;

        private:

            // info
            std::vector<int>      m_TriangulationIndexes      {std::vector<int>()     };
            std::vector<gs_vec2f> m_MeshGeneratorPointsBuffer {std::vector<gs_vec2f>()};
            std::vector<gs_color> m_MeshGeneratorColorsBuffer {std::vector<gs_color>()};
            std::vector<gs_vec2f> m_MeshGeneratorPerturbations{std::vector<gs_vec2f>()};

            // service methods
            gs_vec2f get_perturbation(int _Index)
            {
                for (int i = m_MeshGeneratorPerturbations.size(); i < _Index; i++)
                {
                    m_MeshGeneratorPerturbations.push_back( gs_pseudo_random<float>() );
                }
                
            }
        };
    }
}