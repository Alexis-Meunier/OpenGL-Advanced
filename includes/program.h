#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
#include <vector>

#include "matrix.h"
#include "shader.h"

class Program {
    public:
        Program();

        void attach_shader(const std::string& shd_path, const ShaderType& type);
        void attach_compiled_shader(std::shared_ptr<Shader>& shader);

        // TODO: Do it properly
        void set_uniform(const std::string& name);

        void set_uniform_mat4(const std::string& name, const matrix4& m) const;
        void set_uniform_vec3(const std::string& name, const vec3& v) const;

        void link();
        void bind() const;
    private:
        GLuint _index;
        bool _linked;
        std::vector<std::shared_ptr<Shader>> _shaders;
};

#endif // PROGRAM_H
