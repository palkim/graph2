#include "texture.h"
#include <string>
#include <iostream>

namespace fst
{
  Texture::Texture() {}
  Texture::Texture(int width,
                   int height,
                   unsigned char *image,
                   std::string imageName,
                   std::string interpolation,
                   std::string decalMode,
                   std::string appearance)
      : m_width(width),
        m_height(height),
        m_image(image),
        m_imageName(imageName),
        m_interpolation(interpolation),
        m_decalMode(decalMode),
        m_appearance(appearance)
  {
  }
  std::string Texture::getDecalMode(){
    return m_decalMode;
  }
  math::Vector3f Texture::getColor(float u, float v) {
    if (m_appearance == "Clamp") {
      if (u > 1.0f) {
        u = 1.0f;
      } else if (u < 0.0f) {
        u = 0.0f;
      }
      if (v > 1.0f) {
        v = 1.0f;
      } else if (v < 0.0f) {
        v = 0.0f;
      }
    } else if (m_appearance == "Repeat") {
      u = u - floor(u);
      v = v - floor(v);
    }
    //find i and j
    float i = u * m_width;
    float j = v * m_height;
    // if u or v comes equal to 1
    if (i >= m_width) {
      i = i - 1;
    }
    if (j >= m_height) {
      j = j - 1;
    }
    math::Vector3f C;
    if (m_interpolation == "nearest") {
      i = round(i);
      j = round(j);
      unsigned int index = j * m_width * 3 + i * 3;

      C.x = m_image[index];
      C.y = m_image[index+1];
      C.z = m_image[index+2];

    } else if(m_interpolation == "bilinear") {
      unsigned int p = floor(i);
      unsigned int q = floor(j);
      float dx = i - p;
      float dy = j - q;
      unsigned int index = q * m_width * 3 + p * 3;
      C.x = m_image[index] * (1-dx) * (1 - dy); 
      C.x = C.x + m_image[index + 3] * dx * (1-dy);
      C.x = C.x + m_image[index + m_width * 3] * (1-dx) * dy; 
      C.x = C.x + m_image[index + 3 + m_width * 3] * dx * dy; 

      C.y = m_image[index+1] * (1-dx) * (1 - dy); 
      C.y = C.y + m_image[index + 3 + 1] * dx * (1-dy); 
      C.y = C.y + m_image[index + m_width * 3 + 1] * (1-dx) * dy; 
      C.y = C.y + m_image[index + 3 + m_width * 3 + 1] * dx * dy; 
      
      C.z = m_image[index+2] * (1-dx) * (1 - dy); 
      C.z = C.z + m_image[index + 3 + 2] * dx * (1-dy); 
      C.z = C.z + m_image[index + m_width * 3 + 2] * (1-dx) * dy; 
      C.z = C.z + m_image[index + 3 + m_width * 3 + 2] * dx * dy; 
    }

    if (m_decalMode == "replace_kd" || m_decalMode == "blend_kd") {
      C = C / 255.0f;
    }
    
    
    return C;
  }
} // namespace fst