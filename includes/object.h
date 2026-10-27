#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include <vector>

#include "light.h"
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

        void render(const matrix4& view, const matrix4& proj, const vec3& cam_pos, const float& anim_time, const float& nb_div, const float& middle_div, std::vector<Light>& lights) const;
        void translate(const vec3& t) { _translation = t; update_model(); }
        void scale(const vec3& s) { _scale = s; update_model(); }

    private:
        Buffer _vertices;
        Buffer _normals;
        std::shared_ptr<Material> _material;
        void update_model();
        vec3 _translation = vec3(0.f, 0.f, 0.f);
        vec3 _scale = vec3(1.f, 1.f, 1.f);
        matrix4 _model = matrix4::identity();
};

#endif // OBJECT_H
