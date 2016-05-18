#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include <stdexcept>
#include "Shader.h"


    class GLSLProgram {
    public:

        GLSLProgram();

        // create program, attach shaders, check program
        void create_program(const std::vector<Shader> shaders);

        ~GLSLProgram();

        //this function return the programID
        GLuint get_id() const;

        //this function return the handle of arrribute variable
        GLint attrib(const GLchar *attribName) const;

//this function return the handle of uniform variable
        GLint uniform(const GLchar *uniformName) const;

    private:
        GLuint programID;
    };
#endif
