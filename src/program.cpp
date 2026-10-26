#include "program.h"

#define SHADER_PATH std::string("shaders/")


Program::Program()
{
    // TODO: Error check
    _index = glCreateProgram();
    _linked = false;
}

void Program::set_uniform(const std::string& name)
{
    glProgramUniform3f(_index, glGetUniformLocation(_index, name.c_str()), 1.0, 1.0, 1.0);
}

void Program::attach_shader(const std::string& shd_path, const ShaderType& type)
{
    if (_linked)
    { 
        std::cerr << "Program is already linked, can't add a new shader." << std::endl;
        return; 
    }

    auto shd = std::make_shared<Shader>(SHADER_PATH + shd_path, type);
    _shaders.push_back(shd);

    glAttachShader(_index, shd->_index);
}

void Program::attach_compiled_shader(std::shared_ptr<Shader>& shader)
{
    if (_linked)
    { 
        std::cerr << "Program is already linked, can't add a new shader." << std::endl;
        return; 
    }

    _shaders.push_back(shader);

    glAttachShader(_index, shader->_index);
}

void Program::link()
{
    if (_linked)
    { 
        std::cerr << "Program is already linked." << std::endl; 
        return;
    }
    GLint link_status = GL_TRUE;
    glLinkProgram(_index);
    glGetProgramiv(_index, GL_LINK_STATUS, &link_status);
    // TODO: Prettify this & Delete shaders
    if (link_status != GL_TRUE)
    {
        GLint log_size;
        char *program_log;
        glGetProgramiv(_index, GL_INFO_LOG_LENGTH, &log_size);
        program_log = (char*)std::malloc(log_size+1);
        if(program_log != 0) {
            glGetProgramInfoLog(_index, log_size, &log_size, program_log);
            std::cerr << "Program " << program_log << std::endl;
            std::free(program_log);
        }
        return;
    }
    _linked = true;

    for (auto& shader : _shaders)
    {
        glDeleteShader(shader->_index);
    }
}

void Program::set_uniform_mat4(const std::string& name, const matrix4& m) const
{
    GLfloat vals[16];
    int i = 0;
    for (const auto& row : m.values)
        for (GLfloat v : row)
            vals[i++] = v;

    glProgramUniformMatrix4fv(_index, glGetUniformLocation(_index, name.c_str()),
                              1, GL_TRUE, vals);
}

void Program::set_uniform_vec3(const std::string& name, const vec3& v) const
{
    glProgramUniform3f(_index, glGetUniformLocation(_index, name.c_str()), v.x, v.y, v.z);
}

void Program::bind() const
{
    glUseProgram(_index);
}
