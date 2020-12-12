#pragma once

#include "vector3f.h"
#include "matrix.h"
#include <string>

#include <vector>
namespace fst
{
    class Ray;
    struct HitRecord;

    class Sphere
    {
    public:
        Sphere(const math::Vector3f& center, float radius, int material_id, std::string transformations, int texture_id);

        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const;
        bool intersectShadowRay(const Ray& ray, float max_distance) const;
        std::vector<float> getTextCoord(math::Vector3f p) const;

        math::Vector3f m_center;
        float m_radius;
        int m_material_id;
        std::string m_transformations;
        int m_texture_id;
        math::Vector3f m_u, m_v, m_w;
        Matrix m_align;
    };
}