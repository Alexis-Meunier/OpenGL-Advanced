#include "object.h"

#include <algorithm>

// Object::Object(const std::string& obj_path)
// {
//     // TODO: stb_load or whatever
//     _material = NULL;
// }

Object::Object(const std::vector<GLfloat>& vertices, const std::vector<GLfloat>& normals, const std::shared_ptr<Material>& material)
{
    _vertices.val = vertices;
    _normals.val = normals;
    _material = material;

    glGenBuffers(1, &_vertices.index);
    glBindBuffer(GL_ARRAY_BUFFER, _vertices.index);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &_normals.index);
    glBindBuffer(GL_ARRAY_BUFFER, _normals.index);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);
}

void Object::update_model()
{
    _model = translation(_translation);
    _model *= scaling(_scale);
}

void Object::render(const matrix4& view, const matrix4& proj, const vec3& cam_pos, const float& anim_time, const float& nb_div, const float& middle_div, std::vector<Light>& lights) const
{
    _material->bind();
    _material->set_mat4("view", view);
    _material->set_mat4("proj", proj);
    _material->set_vec3("cameraPos", cam_pos);
    _material->set_float("anim_time", anim_time);
    if (_material->is_tesselation())
    {
        _material->set_float("nb_divisions", nb_div);
        _material->set_float("middle_divisions", middle_div);
    }
    _material->set_lights(lights);
    _material->set_mat4("model", _model);

    glBindBuffer(GL_ARRAY_BUFFER, _vertices.index);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _normals.index);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // glDrawArrays(
    //     _material->draw_type,
    //     0,
    //     _vertices.val.size() / 3);
    glDrawArraysInstanced(_material->draw_type, 0,
                            _vertices.val.size() / 3, _material->instance_count);

}
