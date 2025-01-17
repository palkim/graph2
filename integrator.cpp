#include "integrator.h"
#include "image.h"
#include "ctpl_stl.h"

#include <iostream>
#include <string>

namespace fst
{
    Integrator::Integrator(const parser::Scene& parser)
    {
        m_scene.loadFromParser(parser);
    }

    math::Vector3f Integrator::renderPixel(const Ray& ray, int depth) const
    {
        if (depth > m_scene.max_recursion_depth)
        {
            return math::Vector3f(0.0f, 0.0f, 0.0f);
        }

        HitRecord hit_record;
        auto result = m_scene.intersect(ray, hit_record, std::numeric_limits<float>::max());

        if (!result)
        {
            return m_scene.background_color;
        }

        auto& material = m_scene.materials[hit_record.material_id - 1];
        //ambient shading
        auto color = material.get_ambient() * m_scene.ambient_light;
        auto intersection_point = ray.getPoint(hit_record.distance);
        auto texture_id = hit_record.texture_id;
        

        for (auto& light : m_scene.point_lights)
        {
            auto to_light = light.get_position() - intersection_point;
            auto light_pos_distance = math::length(to_light);
            to_light = to_light / light_pos_distance;

            Ray shadow_ray(intersection_point + m_scene.shadow_ray_epsilon * to_light, to_light);

            if (!m_scene.intersectShadowRay(shadow_ray, light_pos_distance))
            {
                //compute diffuse and specular shading
                
                if (texture_id == -1) {
                    color = color + light.computeRadiance(light_pos_distance) * material.computeBrdf(to_light, -ray.get_direction(), hit_record.normal);
                } else {
                    
                    auto material_texture = m_scene.textures[texture_id - 1];
                    std::string texture_decal_mode = material_texture.getDecalMode();
                    
                    math::Vector3f C = material_texture.getColor(hit_record.u, hit_record.v);
                    
                    if (texture_decal_mode == "replace_kd") {
                        color = color + light.computeRadiance(light_pos_distance) * (C * material.computeDiffuse_without_kd(to_light, -ray.get_direction(), hit_record.normal)); 
                    } else if (texture_decal_mode == "blend_kd") {
                        math::Vector3f kd = material.get_diffuse();
                        math::Vector3f res = (kd + C) / 2;
                        color = color + light.computeRadiance(light_pos_distance) * (res * material.computeDiffuse_without_kd(to_light, -ray.get_direction(), hit_record.normal));
                    } else if (texture_decal_mode == "replace_all") {
                        color = color + C;
                    }
                    color = color + light.computeRadiance(light_pos_distance) * material.computeSpecular(to_light, -ray.get_direction(), hit_record.normal);

                }
            }
        }

        auto& mirror = material.get_mirror();
        if (mirror.x + mirror.y + mirror.z > 0.0f)
        {
            auto new_direction = math::reflect(ray.get_direction(), hit_record.normal);
            Ray secondary_ray(intersection_point + m_scene.shadow_ray_epsilon * new_direction, new_direction);

            return color + mirror * renderPixel(secondary_ray, depth + 1);
        }
        else
        {
            return color;
        }
    }

    void Integrator::integrate() const
    {
        for (auto& camera : m_scene.cameras)
        {
            auto& resolution = camera.get_screen_resolution();
            Image image(resolution.x, resolution.y);

            ctpl::thread_pool pool(128);
            for (int i = 0; i < resolution.x; ++i)
            {
                pool.push([i, &resolution, &camera, &image, this](int id) {
                    for (int j = 0; j < resolution.y; ++j)
                    {
                        auto ray = camera.castPrimayRay(static_cast<float>(i), static_cast<float>(j));
                        auto color = renderPixel(ray, 0);
                        image.setPixel(i, j, math::clamp(color, 0.0f, 255.0f));
                    }
                });
            }
            pool.stop(true);

            image.save(camera.get_image_name());
        }
    }
}
