#ifndef KAWASEFILTER_H
#define KAWASEFILTER_H

#include <GL/glew.h>
#include <vector>
#include "Shader.h"
#include "GLSLProgram.h"
#include <memory>
#include <cstring>
#include <cstdlib>
#include "GaussianBlur.h"

struct ObjectTex {
    GLuint id_vbo, id_tex, id_fbo, id_rbo_c, id_rbo_d;
    GLsizei width, heigth, n_channel;
};

class GaussianFilter {

public:
    GaussianFilter(int width_, int height_, int n_channel_, float sigma_);

    void create_vao();

    void load_shader();

    void blur(unsigned char *image_src, unsigned char *image_dest);


private:
    const int K = 2;
    int width, heigth, n_channel, n_pixel, n_total;
    std::vector<unsigned char> buffer;
    float sigma;
    ObjectTex obj_tex;
    GLSLProgram texture_program;
};


#endif
