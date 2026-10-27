#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
#include <vector>

#include "light.h"
#include "matrix.h"
#include "shader.h"

class Program {
    public:
        Program();

        void attach_shader(const std::string& shd_path, const ShaderType& type);
        void attach_compiled_shader(std::shared_ptr<Shader>& shader);
        void attach_tess_shader(const std::string& tess_ctrl_path, const std::string& tess_eval_path);
        void attach_geometry_shader(const std::string& geo_path);
        void attach_compute_shader(const std::string& compute_path);

        void set_uniform_mat4(const std::string& name, const matrix4& m) const;
        void set_uniform_vec3(const std::string& name, const vec3& v) const;
        void set_uniform_float(const std::string& name, float v) const;
        void set_uniform_int(const std::string& name, int v) const;
        void set_lights(const std::vector<Light>& lights) const;

        GLuint get_index();

        void link();
        void bind() const;
        bool has_tesselation = false;
        bool has_geometry = false;
        bool has_compute = false;
        bool geometry_inactive = true;
    private:
        GLuint _index;
        bool _linked;
        std::vector<std::shared_ptr<Shader>> _shaders;
};

#endif // PROGRAM_H
