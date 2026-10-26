#include "shader.h"

#include <iostream>
#include <fstream>

std::string load(const std::string &filename) {
    std::ifstream input_src_file(filename, std::ios::in);
    if (input_src_file.fail()) {
        std::cerr << "FAIL\n";
        return "";
    }

    std::string ligne;
    std::string file_content="";
    while(getline(input_src_file, ligne)) {
        file_content = file_content + ligne + "\n";
    }
    file_content += '\0';
    input_src_file.close();
    return file_content;
}

Shader::Shader(const std::string& shd_path, const ShaderType& type)
{
    _type = type;
    _index = glCreateShader(_type);

    std::string content = load(shd_path);
    const char* c_str = content.c_str();

    glShaderSource(_index, 1, (const GLchar**)&(c_str), 0);
    glCompileShader(_index);

    int res = 0;
    glGetShaderiv(_index, GL_COMPILE_STATUS, &res);
    if (!res)
    {
        int len = 0;
        char log[1024] = { 0 };
        glGetShaderInfoLog(_index, sizeof(log), &len, log);
        // TODO: Exit ?
        std::cerr << "ERROR: During compilation of Shader: " << shd_path << "\n" << log << std::endl;
    }
}
