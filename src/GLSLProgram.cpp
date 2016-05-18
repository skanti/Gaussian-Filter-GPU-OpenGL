//
// Created by amon on 08/03/16.
//

#include "GLSLProgram.h"

GLSLProgram::GLSLProgram() : programID(0) { };

void GLSLProgram::create_program(const std::vector<Shader> shaders) {
    programID = glCreateProgram();
    if (programID == 0)
        throw std::runtime_error("glCreateProgram failed");

    //attach all the shaders
    for (auto s : shaders)
        glAttachShader(programID, s.ReturnShaderID());

    //link the shaders together
    glLinkProgram(programID);

    //detach all the shaders
    for (auto s : shaders)
        glDetachShader(programID, s.ReturnShaderID());

    //throw exception if linking failed
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");

        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programID, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteProgram(programID);
        programID = 0;
        throw std::runtime_error(msg);
    }
}

GLSLProgram::~GLSLProgram() {
    if (programID != 0) glDeleteProgram(programID);
}

//this function return the programID
GLuint GLSLProgram::get_id() const {
    return programID;
}

//this function return the handle of arrribute variable
GLint GLSLProgram::attrib(const GLchar *attribName) const {
    GLint attrib = glGetAttribLocation(programID, attribName);
    if (attrib == -1)
        throw std::runtime_error(std::string("GLSLProgram attribute not found: ") + attribName);
    return attrib;
}

//this function return the handle of uniform variable
GLint GLSLProgram::uniform(const GLchar *uniformName) const {
    GLint uniform = glGetUniformLocation(programID, uniformName);
    if (uniform == -1)
        throw std::runtime_error(std::string("GLSLProgram uniform not found: ") + uniformName);

    return uniform;
}


