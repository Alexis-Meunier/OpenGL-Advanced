#include "material.h"

Material::Material(const Pipeline& shaders)
{
    _program = std::make_shared<Program>();
    if (!shaders.vrtx_path.empty())
        _program->attach_shader(shaders.vrtx_path, VERT);
    if (!shaders.frag_path.empty())
        _program->attach_shader(shaders.frag_path, FRAG);

    if (!shaders.tess_ctrl_path.empty() && !shaders.tess_eval_path.empty())
    {
        draw_type = GL_PATCHES;
        _program->attach_tess_shader(shaders.tess_ctrl_path, shaders.tess_eval_path);
    }
    if (!shaders.geo_path.empty())
        _program->attach_geometry_shader(shaders.geo_path);
    if (!shaders.compute_path.empty())
        _program->attach_compute_shader(shaders.compute_path);
    _program->link();
}

void Material::attach_program(std::shared_ptr<Program>& program)
{
    _program = program;
}


void Material::set_int(const std::string& name, int v) const
{
    _program->set_uniform_int(name, v);
}

void Material::set_lights(const std::vector<Light>& lights) const
{
    _program->set_lights(lights);
}

void Material::set_float(const std::string& name, float v) const
{
    _program->set_uniform_float(name, v);
}

void Material::set_mat4(const std::string& name, const matrix4& m) const
{
    _program->set_uniform_mat4(name, m);
}

bool Material::is_compute()
{
    return _program->has_compute;
}

bool Material::is_tesselation()
{
    return _program->has_tesselation;
}

void Material::set_vec3(const std::string& name, const vec3& v) const
{
    _program->set_uniform_vec3(name, v);
}

void Material::add_texture(const std::string& uniform, GLuint id)
{
    _textures.push_back({uniform, id});
}

void Material::bind() const
{
    // TODO: Set drawing parameters such as blend etc...
    if (cull_face)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    // TODO:
    // for (const auto& [h, v] : _uniforms)
    // {
    //     _program->set_uniform("intensity");
    // }
    _program->bind();
    for (size_t i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _textures[i].second);
        _program->set_uniform_int(_textures[i].first, i);
    }
}
