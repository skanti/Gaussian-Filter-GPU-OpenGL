# Gaussian-Filter-GPU-OpenGL
This library allows you to apply Gaussian filtering on 2D images via OpenGL.

## Description
If you want to make use of your GPU (say your laptop GPU) then you will notice that everything but a NVIDIA card will be hard 
to access. CUDA allows GPGPU computation for NVIDIA cards only, but it is possible to conduct computation on the GPU 
via OpenGL. In this library you can apply Gaussian Convolution on contiguous 2D data.

## How?
Here I "abused" the Fragment Shader as compute kernel in order to apply Gaussian Convolution on "texture data". The trick
is to instantate an off-screen buffer. That is, instead of drawing to the window buffer of your laptop screen, we will instead
have a background buffer that will undergo the computation in the pipeline. At the end, we can read on the cooked data.

## Requirements
- OpenGL 4
- GLFW
- Intel Compiler ICPC (optional)

## Compability
This works literally on any computer that has a GPU (and satisfies afromentioned requirements).

## Examples
I shipped it with some examples. You can even time it against the OpenCV library. On big images this OpenGL Gaussian Filter
will be faster.
