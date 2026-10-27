#include "program.h"

#define SHADER_PATH std::string("shaders/")


Program::Program()
{
    // TODO: Error check
    _index = glCreateProgram();
    _linked = false;
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

void Program::attach_tess_shader(const std::string& tess_ctrl_path, const std::string& tess_eval_path)
{
    if (_linked)
    { 
        std::cerr << "Program is already linked, can't add a new shader." << std::endl;
        return; 
    }

    auto tess_ctrl_shd = std::make_shared<Shader>(SHADER_PATH + tess_ctrl_path, TESS_CTRL);
    _shaders.push_back(tess_ctrl_shd);
    auto tess_eval_shd = std::make_shared<Shader>(SHADER_PATH + tess_eval_path, TESS_EVAL);
    _shaders.push_back(tess_eval_shd);

    has_tesselation = true;
    glAttachShader(_index, tess_ctrl_shd->_index);
    glAttachShader(_index, tess_eval_shd->_index);
}

void Program::attach_geometry_shader(const std::string& geo_path)
{
    if (_linked)
    { 
        std::cerr << "Program is already linked, can't add a new shader." << std::endl;
        return; 
    }

    auto shd = std::make_shared<Shader>(SHADER_PATH + geo_path, GEO);
    _shaders.push_back(shd);

    has_geometry = true;
    glAttachShader(_index, shd->_index);
}

void Program::attach_compute_shader(const std::string& compute_path)
{
    if (_linked)
    { 
        std::cerr << "Program is already linked, can't add a new shader." << std::endl;
        return; 
    }

    auto shd = std::make_shared<Shader>(SHADER_PATH + compute_path, COMP);
    _shaders.push_back(shd);

    has_compute = true;
    glAttachShader(_index, shd->_index);
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

GLuint Program::get_index()
{
    return _index;
}

void Program::set_uniform_float(const std::string& name, float v) const
{
    glProgramUniform1f(_index, glGetUniformLocation(_index, name.c_str()), v);
}

void Program::set_uniform_int(const std::string& name, int v) const
{
    glProgramUniform1i(_index, glGetUniformLocation(_index, name.c_str()), v);
}

void Program::set_lights(const std::vector<Light>& lights) const
{
    int n = lights.size();
    set_uniform_int("nb_lights", n);
    for (int i = 0; i < n; i++)
    {
        std::string p = "lights[" + std::to_string(i) + "].";
        set_uniform_vec3(p + "pos", lights[i].pos);
        set_uniform_vec3(p + "dir", lights[i].dir);
        set_uniform_vec3(p + "color", lights[i].color);
        set_uniform_float(p + "intensity", lights[i].intensity);
        set_uniform_int(p + "type", lights[i].type);
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
