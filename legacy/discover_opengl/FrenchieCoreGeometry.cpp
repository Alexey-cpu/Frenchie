#include <FrenchieCoreGeometry.hpp>

using namespace Frenchie::Core;

namespace Frenchie
{
    namespace Core
    {
        // Cube
        struct Cube final
        {
        public:

            Cube(glm::vec3 _Center = glm::vec3(0.f), float _Width = 100.f, float _Height = 100.f, float _Thickness = 100.f) : 
                Center(_Center), 
                Width(_Width), 
                Height(_Height), 
                Thickness(_Thickness){}

            ~Cube(){}

            std::vector<glm::vec3> get_points() const
            {
                glm::vec3 P0 = Center + glm::vec3(-Width, +Height, 0.f) * 0.5f;
                glm::vec3 P1 = Center + glm::vec3(+Width, +Height, 0.f) * 0.5f;
                glm::vec3 P2 = Center + glm::vec3(+Width, -Height, 0.f) * 0.5f;
                glm::vec3 P3 = Center + glm::vec3(-Width, -Height, 0.f) * 0.5f;

                return
                {
                    glm::vec3(P0.x, P0.y, +Thickness * 0.5f),
                    glm::vec3(P1.x, P1.y, +Thickness * 0.5f),
                    glm::vec3(P2.x, P2.y, +Thickness * 0.5f),
                    glm::vec3(P3.x, P3.y, +Thickness * 0.5f),
                    glm::vec3(P0.x, P0.y, -Thickness * 0.5f),
                    glm::vec3(P1.x, P1.y, -Thickness * 0.5f),
                    glm::vec3(P2.x, P2.y, -Thickness * 0.5f),
                    glm::vec3(P3.x, P3.y, -Thickness * 0.5f),
                };
            }

            std::vector<Triangle> get_triangles() const
            {
                glm::vec3 P0 = Center + glm::vec3(-Width, +Height, 0.f) * 0.5f;
                glm::vec3 P1 = Center + glm::vec3(+Width, +Height, 0.f) * 0.5f;
                glm::vec3 P2 = Center + glm::vec3(+Width, -Height, 0.f) * 0.5f;
                glm::vec3 P3 = Center + glm::vec3(-Width, -Height, 0.f) * 0.5f;

                glm::vec3 A = glm::vec3(P0.x, P0.y, +Thickness * 0.5f);
                glm::vec3 B = glm::vec3(P1.x, P1.y, +Thickness * 0.5f);
                glm::vec3 C = glm::vec3(P2.x, P2.y, +Thickness * 0.5f);
                glm::vec3 D = glm::vec3(P3.x, P3.y, +Thickness * 0.5f);
                glm::vec3 E = glm::vec3(P0.x, P0.y, -Thickness * 0.5f);
                glm::vec3 F = glm::vec3(P1.x, P1.y, -Thickness * 0.5f);
                glm::vec3 G = glm::vec3(P2.x, P2.y, -Thickness * 0.5f);
                glm::vec3 H = glm::vec3(P3.x, P3.y, -Thickness * 0.5f);

                return
                {
                    // front
                    Triangle(A, B, D),
                    Triangle(D, B, C),

                    // back
                    Triangle(E, F, H),
                    Triangle(H, F, G),

                    // left
                    Triangle(A, E, D),
                    Triangle(E, H, D),

                    // right
                    Triangle(B, F, C),
                    Triangle(F, G, C),

                    // top
                    Triangle(A, E, F),
                    Triangle(F, B, A),

                    // bottom
                    Triangle(D, H, G),
                    Triangle(G, C, D),
                };
            }

            glm::vec3 Center    = glm::vec3(0.f); 
            float     Width     = 100.f; 
            float     Height    = 100.f; 
            float     Thickness = 100.f;
        };
    }
}

// Ray
Ray::Ray(const glm::vec3& _Origin, const glm::vec3& _Direction) : 
    Origin(_Origin), 
    Direction(_Direction), 
    InverseDirection(1.f / Direction.x, 1.f / Direction.y, 1.f / Direction.z){}

Ray::~Ray(){}

Ray Ray::transform(const glm::mat4& _Transform) const
{
    return Ray(
        _Transform * glm::vec4(Origin, 1.f), 
        glm::normalize(_Transform * glm::vec4(Direction, 1.f)));
}

// Triangle
Triangle::Triangle(glm::vec3 _A, glm::vec3 _B, glm::vec3 _C) : A(_A), B(_B), C(_C){}
Triangle::~Triangle(){}

std::vector<glm::vec3> Triangle::get_points() const
{
    return {A, B, C};
}

Triangle Triangle::transform(const glm::mat4& _Transform) const
{
    return Triangle(
        _Transform * glm::vec4(A, 1.f), 
        _Transform * glm::vec4(B, 1.f), 
        _Transform * glm::vec4(C, 1.f)
    );
}

bool Triangle::intersects(const Ray& _Ray) const
{
    // retrieve ray origin point and direction vector
    auto O = _Ray.Origin;
    auto R = _Ray.Direction;

    // get trianle plain's normal
    auto BA = B - A;
    auto AC = A - C;
    auto CB = C - B;
    auto N  = glm::normalize(glm::cross(BA, AC));
    
    // check if triangle and ray are parallel
    // (triangle and ray are parallel if the trianle normal is perpendicular to the ray)
    if(std::abs(glm::dot(N, R)) < 4.f * std::numeric_limits<float>::min()) 
        return false;

    // find the point where the ray hits trianle plain
    auto  D = -glm::dot(N, A);
    float t = -(glm::dot(N, O) + D) / dot(N, R);
    auto  P = O + t * R;

    // Check if the triangle is behind the ray
    if (t < 0) 
        return false;

    // check if the hit point belongs to triangle
    if((glm::dot(N, glm::cross(AC, (C - P)))) < 0.f) 
        return false;

    if((glm::dot(N, glm::cross((P - B), CB))) < 0.f)
        return false;

    if((glm::dot(N, glm::cross((P - A), BA))) < 0.f) 
        return false;

    return true;
}

// Aabb
Aabb::Aabb(const glm::vec3& _Min, const glm::vec3& _Max) : Min(_Min), Max(_Max){}
Aabb::~Aabb(){}

std::vector<glm::vec3> Aabb::get_points() const
{
    auto center = get_center();
    auto size   = get_size();
    return Cube(center, size.x, size.y, size.z).get_points();
}

std::vector<Triangle> Aabb::get_triangles() const
{
    auto center = get_center();
    auto size   = get_size();
    return Cube(center, size.x, size.y, size.z).get_triangles();
}

glm::vec3 Aabb::get_size() const
{
    return (Max - Min);
}

glm::vec3 Aabb::get_center() const
{
    return (Max + Min) * 0.5f;
}

bool Aabb::intersects(const Ray& _Ray) const
{
    double tmin = -INFINITY;
    double tmax = +INFINITY;

    for (int i = 0; i < 3; ++i) 
    {
        if (_Ray.Direction[i] != 0.0) 
        {
            double t1 = (Min[i] - _Ray.Origin[i]) / _Ray.Direction[i];
            double t2 = (Max[i] - _Ray.Origin[i]) / _Ray.Direction[i];

            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
        } 
        else if (_Ray.Origin[i] < Min[i] || _Ray.Origin[i] > Max[i]) // ray origin is outside the box
        {
            return false;
        }
    }

    return tmax > tmin;
}

Aabb Aabb::transform(const glm::mat4& _Transform) const
{
    std::vector<glm::vec3> points = get_points();
    glm::vec3              max    = glm::vec3(-INFINITY);
    glm::vec3              min    = glm::vec3(+INFINITY);

    for(auto&& point : points)
    {
        point = _Transform * glm::vec4(point, 1.f);

        max = glm::vec3(
            std::max<float>(max.x, point.x), 
            std::max<float>(max.y, point.y), 
            std::max<float>(max.z, point.z));

        min = glm::vec3(
            std::min<float>(min.x, point.x), 
            std::min<float>(min.y, point.y), 
            std::min<float>(min.z, point.z));
    }

    return Aabb(min, max);
}

Aabb Aabb::unite(const Aabb& _Other) const
{
    return Aabb(
        glm::vec3(std::min(_Other.Min.x, Min.x), std::min(_Other.Min.y, Min.y), std::min(_Other.Min.z, Min.z)),
        glm::vec3(std::max(_Other.Max.x, Max.x), std::max(_Other.Max.y, Max.y), std::max(_Other.Max.z, Max.z))
    );
}

Aabb Aabb::grow(const glm::vec3& _Size) const
{
    return Aabb(Min - _Size, Max + _Size);
}