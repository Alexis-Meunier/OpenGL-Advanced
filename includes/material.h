#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <memory>

#include "matrix.h"
#include "program.h"

struct Pipeline {
    std::string vrtx_path;
    std::string tess_ctrl_path;
    std::string tess_eval_path;
    std::string geo_path;
    std::string frag_path;
    std::string compute_path;
};

class Material {
    public:
        Material() = delete;
        Material(const Pipeline& shaders);

        void attach_program(std::shared_ptr<Program>& program);

        void bind() const;

        void set_mat4(const std::string& name, const matrix4& m) const;
        void set_vec3(const std::string& name, const vec3& v) const;
        void set_float(const std::string& name, float v) const;
        void set_int(const std::string& name, int v) const;
        void set_lights(const std::vector<Light>& lights) const;

        bool is_compute();
        bool is_tesselation();
        void add_texture(const std::string& uniform, GLuint id);

        GLuint draw_type = GL_TRIANGLES;
    private:
        std::shared_ptr<Program> _program;
        std::vector<std::pair<std::string, GLuint>> _textures;
};

#endif // MATERIAL_H
