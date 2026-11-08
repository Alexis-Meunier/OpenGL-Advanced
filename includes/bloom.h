#pragma once

#include <memory>
#include <vector>

#include "material.h"

class Bloom
{
public:
    float threshold = 1.0f; // brightness where glow starts
    float knee = 0.5f; // softness of the threshold
    float radius = 1.0f; // upsample filter radius (in texels)
    float strength = 0.15f; // how much bloom is added to the scene

    void resize(int width, int height);

    void bind_scene() const;

    void apply();

private:
    struct Mip { GLuint tex = 0; int w = 0, h = 0; };

    static constexpr int N_MIPS = 6;

    void destroy_textures();
    void draw_triangle() const { glDrawArrays(GL_TRIANGLES, 0, 3); }

    int _w = 0;
    int _h = 0;
    GLuint _scene_fbo = 0;
    GLuint _scene_tex = 0;
    GLuint _scene_depth = 0;
    GLuint _mip_fbo = 0;
    std::vector<Mip> _mips;

    std::shared_ptr<Material> _down;
    std::shared_ptr<Material> _up;
    std::shared_ptr<Material> _composite;
};
