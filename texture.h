#pragma once

#include "vector3f.h"
#include <string>

namespace fst
{
    class Texture
    {
    public:
        Texture();
        Texture(int width,
                int height,
                unsigned char *image,
                std::string imageName,
                std::string interpolation,
                std::string decalMode,
                std::string appearance);

        std::string getDecalMode();
        math::Vector3f getColor(float u, float v);

    private:
        int m_width;
        int m_height;
        unsigned char *m_image;
        std::string m_imageName;
        std::string m_interpolation;
        std::string m_decalMode;
        std::string m_appearance;
    };
}