#include "material.h"

Material::Material(const std::string& vrtx_path, const std::string& frag_path)
{
    _program = std::make_shared<Program>();
    _program->attach_shader(vrtx_path, VERT);
    _program->attach_shader(frag_path, FRAG);
    _program->link();
}

void Material::attach_program(std::shared_ptr<Program>& program)
{
    _program = program;
}

void Material::set_mat4(const std::string& name, const matrix4& m) const
{
    _program->set_uniform_mat4(name, m);
}

void Material::bind() const
{
    // TODO: Set drawing parameters such as blend etc...

    // TODO:
    // for (const auto& [h, v] : _uniforms)
    // {
    //     _program->set_uniform("intensity");
    // }
    _program->bind();
}
