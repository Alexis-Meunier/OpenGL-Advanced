#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <vector>

#include "material.h"
#include "matrix.h"

struct Buffer {
    std::vector<GLfloat> val;
    GLuint index = 0;
};

class Object {
    public:
        Object() = default;
        Object(const std::string& obj_path);
        Object(const std::vector<GLfloat>& vertices, const std::vector<GLfloat>& normals, const std::shared_ptr<Material>& material);

        void render(const matrix4& view, const matrix4& proj) const;

    private:
        Buffer _vertices;
        Buffer _normals;
        std::shared_ptr<Material> _material;
};

#endif // OBJECT_H
