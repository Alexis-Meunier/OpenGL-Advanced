#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <memory>

#include "matrix.h"
#include "program.h"

class Material {
    public:
        Material() = delete;
        Material(const std::string& vrtx_path, const std::string& frag_path);

        void attach_program(std::shared_ptr<Program>& program);

        void bind() const;

        void set_mat4(const std::string& name, const matrix4& m) const;

    private:
        std::shared_ptr<Program> _program;
        // TODO: Find type for uniformValue
        // GLuint = uniform location, void* = value (temp)
        // std::vector<std::pair<GLuint, void*>> _uniforms;
};

#endif // MATERIAL_H
