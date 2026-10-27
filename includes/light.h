#ifndef LIGHT_H
#define LIGHT_H

#include "matrix.h"

enum LightType { POINT_LIGHT = 0, DIRECTIONAL_LIGHT = 1 };

class Light {
    public:
        Light();
        Light(const vec3& pos, const vec3& color, const float& intensity);   // point light
        static Light directional(const vec3& dir, const vec3& color, float intensity);

        vec3 pos;
        vec3 dir;
        vec3 color;
        float intensity;
        int type = POINT_LIGHT;
};

#endif // LIGHT_H