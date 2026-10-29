#pragma once

#include <memory>

#include "material.h"
#include "matrix.h"

class Skybox
{
public:
    Skybox();

    void render(const matrix4& view, const matrix4& proj,
                const vec3& sun_dir, const vec3& sun_color) const;

private:
    std::shared_ptr<Material> _material;
};
