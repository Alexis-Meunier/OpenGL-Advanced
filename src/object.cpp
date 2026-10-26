#include "object.h"

Object::Object(const std::string& obj_path)
{
    // TODO: stb_load or whatever
    _material = NULL;
}

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

void Object::render(const matrix4& view, const matrix4& proj) const
{
    _material->bind();
    _material->set_mat4("view", view);
    _material->set_mat4("proj", proj);

    glBindBuffer(GL_ARRAY_BUFFER, _vertices.index);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, _normals.index);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, _vertices.val.size() / 3);
}
