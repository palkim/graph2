#include "hit_record.h"
#include "triangle.h"
#include "ray.h"

namespace fst
{
    Triangle::Triangle(const math::Vector3f& v0, const math::Vector3f& edge1, const math::Vector3f& edge2, const math::Vector2f& ua, const math::Vector2f& ub, const math::Vector2f& uc)
        : m_v0(v0)
        , m_edge1(edge1)
        , m_edge2(edge2)
        , m_normal( math::normalize( math::cross(edge1, edge2) ) )
        , m_ua(ua)
        , m_ub(ub)
        , m_uc(uc)
    {}

    bool Triangle::intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const
    {
        //w2 -> v -> beta
        //w1 -> u -> gamma
        //w = 1 - u - v -> alpha
        //M�ller-Trumbore algorithm
        auto pvec = math::cross(ray.get_direction(), m_edge2);
        auto inv_det = 1.0f / math::dot(m_edge1, pvec);

        auto tvec = ray.get_origin() - m_v0;
        auto w1 = math::dot(tvec, pvec) * inv_det;

        if (w1 < 0.0f || w1 > 1.0f)
        {
            return false;
        }

        auto qvec = math::cross(tvec, m_edge1);
        auto w2 = math::dot(ray.get_direction(), qvec) * inv_det;

        if (w2 < 0.0f || (w1 + w2) > 1.0f)
        {
            return false;
        }

        auto distance = math::dot(m_edge2, qvec) * inv_det;
        if (distance > 0.0f && distance < max_distance)
        {
            //Fill the intersection record.
            hit_record.normal = m_normal;
            hit_record.distance = distance;
            //  calculate u,v for triangle intersection point, put it to hit_record
            hit_record.u = m_ua.x + w2 * (m_ub.x - m_ua.x) + w1 * (m_uc.x - m_ua.x);
            hit_record.v = m_ua.y + w2 * (m_ub.y - m_ua.y) + w1 * (m_uc.y - m_ua.y);

            return true;
        }
        return false;
    }

    bool Triangle::intersectShadowRay(const Ray& ray, float max_distance) const
    {
        //M�ller-Trumbore algorithm
        auto pvec = math::cross(ray.get_direction(), m_edge2);
        auto inv_det = 1.0f / math::dot(m_edge1, pvec);

        auto tvec = ray.get_origin() - m_v0;
        auto w1 = math::dot(tvec, pvec) * inv_det;

        if (w1 < 0.0f || w1 > 1.0f)
        {
            return false;
        }

        auto qvec = math::cross(tvec, m_edge1);
        auto w2 = math::dot(ray.get_direction(), qvec) * inv_det;

        if (w2 < 0.0f || (w1 + w2) > 1.0f)
        {
            return false;
        }

        auto distance = math::dot(m_edge2, qvec) * inv_det;
        return distance > 0.0f && distance < max_distance;
    }

    std::vector<math::Vector3f> Triangle::getVertices(){
        std::vector<math::Vector3f> vertices;
        vertices.push_back(m_v0);
        vertices.push_back(m_v0 + m_edge1);
        vertices.push_back(m_v0 + m_edge2);
        return vertices;
    }

    math::Vector3f Triangle::getNormal(){
        return m_normal;
    }

    void Triangle::setVertices(math::Vector3f v0,math::Vector3f v1, math::Vector3f v2){
        m_v0 = v0;
        m_edge1 = v1-v0;
        m_edge2 = v2-v0;
    }

    void Triangle::setVertices(std::vector<math::Vector3f> vertices){
        m_v0 = vertices[0];
        m_edge1 = vertices[1] - vertices[0];
        m_edge2 = vertices[2] - vertices[0];
    }

    void Triangle::setNormal(math::Vector3f n){
        m_normal = n;
        m_normal = math::normalize(m_normal);
    }
}
