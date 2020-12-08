#pragma once

#include "triangle.h"

#include <vector>
#include <string>

namespace fst
{
    class Mesh : public Triangular
    {
    public:
        Mesh(const std::vector<Triangle>& triangles, int material_id, std::string transformations, int texture_id);
        Mesh(std::vector<Triangle>&& triangles, int material_id, std::string transformations, int texture_id);

        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const override;
        bool intersectShadowRay(const Ray& ray, float max_distance) const override;

    private:
	    std::vector<Triangle> m_triangles;
        int m_material_id;
        std::string m_transformations;
        int m_texture_id; 
    };
}
