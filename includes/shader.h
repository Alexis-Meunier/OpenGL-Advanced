#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

enum ShaderType {
    FRAG        = GL_FRAGMENT_SHADER,         // Fragment Shader
    VERT        = GL_VERTEX_SHADER,           // Vertex Shader
    TESS_CTRL   = GL_TESS_EVALUATION_SHADER,  // Tesselation Control Shader
    TESS_EVAL   = GL_TESS_CONTROL_SHADER,     // Tesselation Evaluation Shader
    GEO         = GL_GEOMETRY_SHADER,         // Geometry Shader
    COMP        = GL_COMPUTE_SHADER,          // Compute Shader
};

class Shader {
    public:
        Shader() = delete;
        Shader(const std::string& shd_path, const ShaderType& type);

    private:
        ShaderType _type;
        GLuint _index;
    
    friend class Program;
};

#endif // SHADER_H
