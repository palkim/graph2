#pragma once

#include "vector3f.h"

namespace fst
{
    struct HitRecord
    {
        math::Vector3f normal;
        float distance;
        int material_id;
        int texture_id;
        float u;
        float v;
    };
}