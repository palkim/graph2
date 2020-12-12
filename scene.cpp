#include "scene.h"
#include <iostream>

namespace fst
{
    void Scene::loadFromParser(const parser::Scene& parser)
    {

        for (auto& camera : parser.cameras)
        {
            cameras.push_back(Camera(
                math::Vector3f(camera.position.x, camera.position.y, camera.position.z),
                math::Vector3f(camera.gaze.x, camera.gaze.y, camera.gaze.z),
                math::Vector3f(camera.up.x, camera.up.y, camera.up.z),
                math::Vector4f(camera.near_plane.x, camera.near_plane.y, camera.near_plane.z, camera.near_plane.w),
                math::Vector2f(camera.image_width, camera.image_height),
                camera.image_name,
                camera.near_distance));
        }

        for (auto& pointlight : parser.point_lights)
        {
            point_lights.push_back(PointLight(
                math::Vector3f(pointlight.position.x, pointlight.position.y, pointlight.position.z),
                math::Vector3f(pointlight.intensity.x, pointlight.intensity.y, pointlight.intensity.z)));
        }

        for (auto& material : parser.materials)
        {
            materials.push_back(Material(
                math::Vector3f(material.ambient.x, material.ambient.y, material.ambient.z),
                math::Vector3f(material.diffuse.x, material.diffuse.y, material.diffuse.z),
                math::Vector3f(material.specular.x, material.specular.y, material.specular.z),
                math::Vector3f(material.mirror.x, material.mirror.y, material.mirror.z),
                material.phong_exponent));
        }

        for (auto &translation : parser.translations)
        {
            translations.push_back(
                Translation(translation.x, translation.y, translation.z));
            tMatrices.push_back(
                Matrix(Translation(translation.x, translation.y, translation.z)));
        }

        for (auto &scaling : parser.scalings)
        {
            scalings.push_back(
                Scaling(scaling.x, scaling.y, scaling.z));
            sMatrices.push_back(Matrix(Scaling(scaling.x, scaling.y, scaling.z)));

        }

        for (auto &rotation : parser.rotations)
        {
            rotations.push_back(
                Rotation(rotation.angle, rotation.x, rotation.y, rotation.z));
            rMatrices.push_back(Matrix(Rotation(rotation.angle, rotation.x, rotation.y, rotation.z)));
            
        }

        for (auto& vertex : parser.vertex_data)
        {
            vertex_data.push_back(math::Vector3f(vertex.x, vertex.y, vertex.z));
        }

        for (auto& tex_coord : parser.tex_coord_data)
        {
            tex_coord_data.push_back(math::Vector2f(tex_coord.x, tex_coord.y));
        }

        for (auto& texture : parser.textures)
        {
            int width;
            int height;
            unsigned char* image;

            std::vector<char> writable(texture.imageName.begin(), texture.imageName.end());
            writable.push_back('\0');

            read_jpeg_header(&*writable.begin(), width, height);
            image = new unsigned char[width * height * 3];
            read_jpeg(&*writable.begin(), image, width, height);
            textures.push_back(Texture(width, height, image, texture.imageName, texture.interpolation, texture.decalMode, texture.appearance));
        }

        for (auto& mesh : parser.meshes)
        {
            std::vector<Triangle> triangles;
            for (auto& face : mesh.faces)
            {
                if (tex_coord_data.size() > 0) {
                    triangles.push_back(Triangle(
                            vertex_data[face.v0_id - 1],
                            vertex_data[face.v1_id - 1] - vertex_data[face.v0_id - 1],
                            vertex_data[face.v2_id - 1] - vertex_data[face.v0_id - 1],
                            tex_coord_data[face.v0_id - 1],
                            tex_coord_data[face.v1_id - 1],
                            tex_coord_data[face.v2_id - 1])
                        );
                } else {
                    triangles.push_back(Triangle(
                            vertex_data[face.v0_id - 1],
                            vertex_data[face.v1_id - 1] - vertex_data[face.v0_id - 1],
                            vertex_data[face.v2_id - 1] - vertex_data[face.v0_id - 1],
                            math::Vector2f(-1),
                            math::Vector2f(-1),
                            math::Vector2f(-1))
                        );
                }
                
            }
            
            meshes.push_back(Mesh(std::move(triangles), mesh.material_id, mesh.transformations, mesh.texture_id));
        }

        for (auto& triangle : parser.triangles)
        {
            std::vector<Triangle> triangles;

            if (tex_coord_data.size() > 0) {
                triangles.push_back(Triangle(
                    vertex_data[triangle.indices.v0_id - 1],
                    vertex_data[triangle.indices.v1_id - 1] - vertex_data[triangle.indices.v0_id - 1],
                    vertex_data[triangle.indices.v2_id - 1] - vertex_data[triangle.indices.v0_id - 1],
                    tex_coord_data[triangle.indices.v0_id - 1],
                    tex_coord_data[triangle.indices.v1_id - 1],
                    tex_coord_data[triangle.indices.v2_id - 1])
                    );
            } else {
                triangles.push_back(Triangle(
                    vertex_data[triangle.indices.v0_id - 1],
                    vertex_data[triangle.indices.v1_id - 1] - vertex_data[triangle.indices.v0_id - 1],
                    vertex_data[triangle.indices.v2_id - 1] - vertex_data[triangle.indices.v0_id - 1],
                    math::Vector2f(-1),
                    math::Vector2f(-1),
                    math::Vector2f(-1))
                    );
            }

            meshes.push_back(Mesh(std::move(triangles), triangle.material_id, triangle.transformations, triangle.texture_id));
        }

        for (auto& sphere : parser.spheres)
        {
            spheres.push_back(Sphere(vertex_data[sphere.center_vertex_id - 1],
                sphere.radius, sphere.material_id, sphere.transformations, sphere.texture_id));
        }

        background_color = math::Vector3f(parser.background_color.x, parser.background_color.y, parser.background_color.z);
        ambient_light = math::Vector3f(parser.ambient_light.x, parser.ambient_light.y, parser.ambient_light.z);
        shadow_ray_epsilon = parser.shadow_ray_epsilon;
        max_recursion_depth = parser.max_recursion_depth;

        // Now, for each object I should calculate its transformation matrix and transform all its vertices and normal vectors.
        // I think we don't deal with triangle structures at all. The code only loops for meshes, interprets triangles as meshes too.
        // So let me start by doing all the transformations for meshes.
        // I think, rotations should be applied without translation ? If not, I don't know according to what 
        // should we translate, CM of the object ? If so how the hell can I calculate it for sth like a horse?
        for (auto & mesh : meshes){
            Matrix allTrans;
            std::string trans = mesh.m_transformations;
            int transSize = trans.size();
            for (int i=0; i<transSize; i++){
                if (trans[i] == 't'){
                    fst::Translation t = translations[fst::math::string2Index(trans,i)];
                    Matrix tmp(t);
                    allTrans = tmp*allTrans;
                }
                else if (trans[i] == 'r'){
                    fst::Rotation r = rotations[fst::math::string2Index(trans,i)];
                    Matrix tmp(r);
                    allTrans = tmp*allTrans;
                }
                else if (trans[i] == 's'){
                    fst::Scaling s = scalings[fst::math::string2Index(trans,i)];
                    Matrix tmp(s);
                    allTrans = tmp*allTrans;
                }
            }
            Matrix normalTrans = transpose(invertt(allTrans));
            for(auto & tr: mesh.m_triangles){
                // Apply allTrans to all vertices and normalTrans to all normals.
                std::vector<math::Vector3f> vertices = tr.getVertices();
                for (int i=0; i<3; i++){
                    vertices[i] = allTrans * vertices[i];
                }
                tr.setVertices(vertices);
                tr.setNormal(normalTrans*tr.getNormal());
            }
        }

        for (auto & sphere : spheres){
            // I think we should apply each transformation to the sphere as it occurs.
            Matrix allTrans;
            std::string trans = sphere.m_transformations;
            int transSize = trans.size();
            std::cout << trans << std::endl;
            for (int i=0; i<transSize; i++){
                if (trans[i] == 't'){
                    // translate only the center
                    fst::Translation t = translations[fst::math::string2Index(trans,i)];
                    sphere.m_center = Matrix(t)*sphere.m_center;
                }
                else if (trans[i] == 'r'){
                    // we should rotate only the uvw coordinate system i guess, but forget it for now
                    fst::Rotation r = rotations[fst::math::string2Index(trans,i)];
                    fst::Matrix rM = Matrix(r);
                    //std::cout << "r\n" << rM << std::endl;
                    //std::cout << sphere.m_u << sphere.m_v << sphere.m_w << "\n";
                    //std::cout << "\nalign\n" << sphere.m_align << "\n";
                    sphere.m_u = normalize(rM*sphere.m_u);
                    sphere.m_v = normalize(rM*sphere.m_v);
                    sphere.m_w = normalize(rM*sphere.m_w);
                    //std::cout << sphere.m_u << sphere.m_v << sphere.m_w << "\n";
                    sphere.m_align = Matrix(sphere.m_u, sphere.m_v, sphere.m_w);
                    //std::cout << "AAalign\n" << sphere.m_align << "\n";
                }
                else if (trans[i] == 's'){
                    fst::Scaling s = scalings[fst::math::string2Index(trans,i)];
                    // We should just multiply the radius by this scaling value ?
                    // ASK ABOUT THE FORM OF THE SCALING, 
                    sphere.m_radius = sphere.m_radius*(s.x);
                }
            }
        }
    }

    bool Scene::intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const
    {
        HitRecord temp;
        float min_distance = max_distance;
        for (auto& sphere : spheres)
        {
            if (sphere.intersect(ray, temp, min_distance))
            {
                min_distance = temp.distance;
                hit_record = temp;
            }
        }

        for (auto& mesh : meshes)
        {
            if (mesh.intersect(ray, temp, min_distance))
            {
                min_distance = temp.distance;
                hit_record = temp;
            }
        }

        return min_distance != max_distance;
    }

    bool Scene::intersectShadowRay(const Ray& ray, float max_distance) const
    {
        for (auto& sphere : spheres)
        {
            if (sphere.intersectShadowRay(ray, max_distance))
            {
                return true;
            }
        }


        for (auto& mesh : meshes)
        {
            if (mesh.intersectShadowRay(ray, max_distance))
            {
		return true;
            }
        }

	return false;
    }
}
