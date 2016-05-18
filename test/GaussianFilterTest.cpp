#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include "GaussianFilter.h"
#include "Timer.h"
#include <GLFW/glfw3.h>

GLFWwindow *offscreen_context;

double time_gaussian(unsigned char *src_image, unsigned char *dest_image, const int width, const int height,
                     const int n_channel) {
    GaussianFilter gf(width, height, n_channel, 2.0);
    Timer::start();
    gf.blur(src_image, dest_image);
    Timer::stop();
    std::cout << "timing (ms): " << Timer::get_timing() << std::endl;
    return Timer::get_timing();
}

double time_gaussian_opencv(cv::Mat &src, cv::Mat &dst) {
    Timer::start();
    cv::GaussianBlur(src, dst, cv::Size(9, 9), 2.0);
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
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
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
    std::vector<std::string> files1 = {{"cake.jpg", "cherry.jpg", "dreamliner.jpg", "macbook.jpg"}};
    std::vector<std::string> files2 = {{"apple.jpg", "astronaut.jpg", "bike.jpg", "bmw.jpg"}};
    cv::Mat warmup = cv::imread("/Users/amon/grive/development/GaussianFilterGPU/picdump/orange.jpg");
    int n = 4;
    printf("n_files: %lu width: 640 heigth: 480 n_channels: 3\n", files1.size());

    double timing_gcv = 0.0;
    cv::Mat out1(480, 640, CV_8UC3);
    time_gaussian_opencv(warmup, out1);
    for (int i = 0; i < n; i++) {
        cv::Mat img = cv::imread("/Users/amon/grive/development/GaussianFilterGPU/picdump/" + files1[i]);
        timing_gcv += time_gaussian_opencv(img, out1);
    }
    printf("Gaussian Filter OpenCV average time (ms): %f\n", timing_gcv / n);

    printf("\n***************************\n");

    double timing_gbf = 0.0;
    cv::Mat out2(480, 640, CV_8UC3);
    time_gaussian(warmup.data, out2.data, warmup.cols, warmup.rows, warmup.channels());
    for (int i = 0; i < n; i++) {
        cv::Mat img = cv::imread("/Users/amon/grive/development/GaussianFilterGPU/picdump/" + files2[i]);
        timing_gbf += time_gaussian(img.data, out2.data, img.cols, img.rows, img.channels());
    }
    printf("Gaussian Filter GPU average time (ms): %f\n", timing_gbf / n);

    printf("\n***************************\n");

    cv::imshow("Display window", out2);
    cv::waitKey(0);

    return 0;
}