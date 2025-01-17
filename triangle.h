#pragma once

#include "triangular.h"
#include "vector3f.h"
#include "vector2f.h"
#include <vector>

namespace fst
{
    class Ray;
    struct HitRecord;

    class Triangle : public Triangular
    {
    public:
        Triangle(const math::Vector3f& v0, const math::Vector3f& edge1, const math::Vector3f& edge2, const math::Vector2f& ua, const math::Vector2f& ub, const math::Vector2f& uc);

        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const override;
        bool intersectShadowRay(const Ray& ray, float max_distance) const override;
        std::vector<math::Vector3f> getVertices();
        void setVertices(math::Vector3f v1,math::Vector3f v2, math::Vector3f v3);
        void setVertices(std::vector<math::Vector3f> vertices);
        void setNormal(math::Vector3f n);
        math::Vector3f getNormal();

    private:
        math::Vector3f m_v0, m_edge1, m_edge2;
        math::Vector3f m_normal;
        math::Vector2f m_ua, m_ub, m_uc;
    };
}
