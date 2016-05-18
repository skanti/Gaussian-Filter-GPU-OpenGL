#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "GaussianFilter.h"
#include "Timer.h"
#include <GLFW/glfw3.h>

GLFWwindow *offscreen_context;

double time_gaussian(unsigned char *src_image, unsigned char *dest_image, const int width, const int height,
                     const int n_channel) {
    GaussianFilter gf(width, height, n_channel, 2.0);
    Timer::start();
    while (!glfwWindowShouldClose(offscreen_context)) {
        // process pending events
        glfwPollEvents();
        // render screen
        gf.blur(src_image, dest_image);
        // draw on screen
        glfwSwapBuffers(offscreen_context);
    }
    // clean up and exit
    glfwTerminate();
    Timer::stop();
    std::cout << "timing (ms): " << Timer::get_timing() << std::endl;
    return Timer::get_timing();
}

void init_opengl() {
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");
    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    //glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
    offscreen_context = glfwCreateWindow(640, 480, "", NULL, NULL);

    // GLFW settings
    glfwMakeContextCurrent(offscreen_context);
    // initialise GLEW

    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed\n");
}

int main() {
    init_opengl();
    std::vector<std::string> files1 = {{"apple.jpg", "astronaut.jpg", "bike.jpg", "bmw.jpg"}};
    //std::vector<std::string> files2 = {{"cake.jpg", "cherry.jpg", "dreamliner.jpg", "macbook.jpg"}};
    int n = 1;
    std::random_shuffle(files1.begin(), files1.end());
    std::random_shuffle(files1.begin(), files1.end());
    printf("n_files: %lu width: 640 heigth: 480 n_channels: 3\n", files1.size());

    double timing_gbf = 0.0;
    cv::Mat out;
    for (int i = 0; i < n; i++) {
        cv::Mat img = cv::imread("/Users/amon/grive/development/GaussianFilterGPU/picdump/" + files1[i]);
        img.copyTo(out);
        timing_gbf += time_gaussian(img.data, out.data, img.cols, img.rows, img.channels());
    }
    printf("Gaussian Filter GPU average time (ms): %f\n", timing_gbf / n);

    //cv::imshow("Display window", out);
    //cv::waitKey(0);

    return 0;
}