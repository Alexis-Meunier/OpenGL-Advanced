#include "light.h"

Light::Light()
    : pos({0.0, 0.0, 0.0}), color({1.0, 1.0, 1.0}), intensity(1.0)
{}

Light::Light(const vec3& pos, const vec3& color, const float& intensity)
    : pos(pos), color(color), intensity(intensity)
{}

Light Light::directional(const vec3& dir, const vec3& color, float intensity)
{
    Light l;
    l.dir = dir; l.color = color; l.intensity = intensity;
    l.type = DIRECTIONAL_LIGHT;
    return l;
}
