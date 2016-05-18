#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>


class Shader {
public:
    Shader();

    GLuint LoadShader(std::string shaderFile, GLenum type);

    GLuint ReturnShaderID() const;

private:
    GLuint shaderID;
};

#endif
