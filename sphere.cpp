#include "sphere.h"
#include "hit_record.h"
#include "ray.h"


namespace fst
{
    Sphere::Sphere(const math::Vector3f& center, float radius, int material_id, std::string transformations, int texture_id)
        : m_center(center)
        , m_radius(radius)
        , m_material_id(material_id)
        , m_transformations(transformations)
        , m_texture_id(texture_id)
        , m_u(1,0,0)
        , m_v(0,1,0)
        , m_w(0,0,1)
        , m_align(m_u, m_v, m_w)
    {}

    bool Sphere::intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const
    {
        //geometrical approach
        auto e = m_center - ray.get_origin();
        auto a = math::dot(e, ray.get_direction());
        auto x = m_radius * m_radius + a * a - math::dot(e, e);

        if (x < 0.0f)
        {
            return false;
        }


        auto distance = a - sqrtf(x);

        
        
        if (distance > 0.0f && distance < max_distance)
        {
            //Fill the intersection record.
            math::Vector3f intersection_point = ray.getPoint(distance);
            std::vector<float> uv_vector = getTextCoord(intersection_point); 
            
            hit_record.distance = distance;
            hit_record.normal = math::normalize(ray.getPoint(hit_record.distance) - m_center);
            hit_record.material_id = m_material_id;
            hit_record.texture_id = m_texture_id;
            //  calculate u,v for sphere intersection point, put it to hit_record
            hit_record.u = uv_vector[0];
            hit_record.v = uv_vector[1];

            //hit_record.u = (-1 * atan2(hit_record.normal.z, hit_record.normal.x) + M_PI) / M_PI / 2;
            //hit_record.v = acos(hit_record.normal.y) / M_PI;

            return true;
        }
        return false;
    }

    bool Sphere::intersectShadowRay(const Ray& ray, float max_distance) const
    {
        //geometrical approach
        auto e = m_center - ray.get_origin();
        auto a = math::dot(e, ray.get_direction());
        auto x = m_radius * m_radius + a * a - math::dot(e, e);

        if (x < 0.0f)
        {
            return false;
        }

        auto distance = a - sqrtf(x);
        return distance > 0.0f && distance < max_distance;
    }
    
    std::vector<float> Sphere::getTextCoord(math::Vector3f p) const{
        math::Vector3f pnew = p - m_center;
        pnew = m_align*pnew;
        std::vector<float> result;
        float phi = acos(pnew.y / m_radius);
        float theta = atan2(pnew.z, pnew.x);
        float u = (-theta + M_PI) / (2*M_PI);
        float v = phi / M_PI;
        result.push_back(u);
        result.push_back(v);
        
        return result;
    }
}