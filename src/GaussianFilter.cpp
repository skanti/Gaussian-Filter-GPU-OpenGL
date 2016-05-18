//
// Created by amon on 5/17/16.
//

#include "GaussianFilter.h"


GaussianFilter::GaussianFilter(int width_, int height_, int n_channel_, float sigma_)
        : width(width_), heigth(height_), n_channel(n_channel_), n_pixel(width * heigth), n_total(n_pixel * n_channel),
          buffer(n_total), sigma(sigma_),
          obj_tex(ObjectTex{0, 0, 0, 0, 0, 0, width, heigth, n_channel}) {


    load_shader();
    create_vao();
}

void GaussianFilter::load_shader() {
    Shader gaussian_vs, gaussian_fs;
    std::string shader_path = "/Users/amon/grive/development/GaussianFilterGPU/src/shader/";
    gaussian_vs.LoadShader(shader_path + "GaussianVS.glsl", GL_VERTEX_SHADER);
    gaussian_fs.LoadShader(shader_path + "GaussianFS.glsl", GL_FRAGMENT_SHADER);
    std::vector<Shader> shaders_texture{{gaussian_vs, gaussian_fs}};
    texture_program.create_program(shaders_texture);
}

void GaussianFilter::create_vao() {
    glUseProgram(texture_program.get_id());

    glGenVertexArrays(1, &obj_tex.id_vao);
    glBindVertexArray(obj_tex.id_vao);

    glViewport(0, 0, (GLint) width, (GLint) heigth);

    glBindFramebuffer(GL_FRAMEBUFFER, obj_tex.id_fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // VBO
    glGenBuffers(1, &obj_tex.id_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, obj_tex.id_vbo);

    GLfloat quad[] = {-1.f, 1.f,
                      1.f, -1.f,
                      1.f, 1.f,
                      -1.f, 1.f,
                      -1.f, -1.f,
                      1.f, -1.f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 6, quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);


    GLuint tex_pos_handler;
    glGenBuffers(1, &tex_pos_handler);
    GLfloat tex_pos[] = {
            // pos
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, tex_pos_handler);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, tex_pos, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);  // Vertex position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);


    glGenTextures(1, &obj_tex.id_tex);
    glBindTexture(GL_TEXTURE_2D, obj_tex.id_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, obj_tex.width, obj_tex.heigth, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /*
    // FBO
    glGenFramebuffers(1, &obj_tex.id_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, obj_tex.id_fbo);

    // RBO color
    glGenRenderbuffers(1, &obj_tex.id_rbo_c);
    glBindRenderbuffer(GL_RENDERBUFFER, obj_tex.id_rbo_c);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, heigth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, obj_tex.id_rbo_c);

    // RBO depth
    glGenRenderbuffers(1, &obj_tex.id_rbo_d);
    glBindRenderbuffer(GL_RENDERBUFFER, obj_tex.id_rbo_d);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, heigth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, obj_tex.id_rbo_d);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Error in Framebuffer RBO" << std::endl;

    // Texture
    glGenTextures(1, &obj_tex.id_tex);
    glBindTexture(GL_TEXTURE_2D, obj_tex.id_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, obj_tex.width, obj_tex.heigth, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, obj_tex.id_tex, 0);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, obj_tex.id_tex, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Error in Framebuffer TEX" << std::endl;

    */

    // Uniforms
    float resolution[2] = {640, 480};
    glUniform2fv(texture_program.uniform("resolution"), 1, resolution);
    float direction[2] = {1, 0};
    glUniform2fv(texture_program.uniform("direction"), 1, direction);


}


void GaussianFilter::blur(unsigned char *image_src, unsigned char *image_dest) {
    glClearColor(0.7, 0.7, 0.7f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, obj_tex.id_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, obj_tex.width, obj_tex.heigth, 0, GL_BGR, GL_UNSIGNED_BYTE, image_src);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

/*
void GaussianFilter::blur(unsigned char *image_src, unsigned char *image_dest) {
    glBindFramebuffer(GL_FRAMEBUFFER, obj_tex.id_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, obj_tex.id_rbo_c);
    glBindTexture(GL_TEXTURE_2D, obj_tex.id_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, obj_tex.width, obj_tex.heigth, 0, GL_BGR, GL_UNSIGNED_BYTE, image_src);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, width, heigth, GL_RGB8, GL_UNSIGNED_BYTE, image_dest);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
 */