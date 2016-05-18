#include <assert.h>
#include <vector>
#include <string>
#include <stdarg.h>
#include <cmath>

inline std::string stringFormatA(const char *fmt, ...) {
    int nSize = 0;
    char buff[4096];
    va_list args;
    va_start(args, fmt);
    nSize = vsnprintf(buff, sizeof(buff) - 1, fmt, args); // C4996
    return std::string(buff);
}

inline std::vector<double> GenerateSeparableGaussKernel(double sigma, int kernelSize) {
    if ((kernelSize % 2) != 1) {
        assert(false); // kernel size must be odd number
        return std::vector<double>();
    }

    int halfKernelSize = kernelSize / 2;

    std::vector<double> kernel;
    kernel.resize(kernelSize);

    const double cPI = 3.14159265358979323846;
    double mean = halfKernelSize;
    double sum = 0.0;
    for (int x = 0; x < kernelSize; ++x) {
        kernel[x] = (float) std::sqrt(
                std::exp(-0.5 * (std::pow((x - mean) / sigma, 2.0) + std::pow((mean) / sigma, 2.0)))
                / (2 * cPI * sigma * sigma));
        sum += kernel[x];
    }
    for (int x = 0; x < kernelSize; ++x)
        kernel[x] /= sum;

    return kernel;
}

inline std::vector<float> GetAppropriateSeparableGauss(int kernelSize) {
    if ((kernelSize % 2) != 1) {
        assert(false); // kernel size must be odd number
        return std::vector<float>();
    }

    // Search for sigma to cover the whole kernel size with sensible values (might not be ideal for all cases quality-wise but is good enough for performance testing)
    const double epsilon = 2e-2f / kernelSize;
    double searchStep = 1.0;
    double sigma = 1.0;
    while (true) {

        std::vector<double> kernelAttempt = GenerateSeparableGaussKernel(sigma, kernelSize);
        if (kernelAttempt[0] > epsilon) {
            if (searchStep > 0.02) {
                sigma -= searchStep;
                searchStep *= 0.1;
                sigma += searchStep;
                continue;
            }
            std::cout << sigma << std::endl;
            std::vector<float> retVal;
            for (int i = 0; i < kernelSize; i++)
                retVal.push_back((float) kernelAttempt[i]);
            return retVal;
        }

        sigma += searchStep;

        if (sigma > 1000.0) {
            assert(false); // not tested, preventing infinite loop
        }
    }
}

inline std::string GenerateGaussShaderKernelWeightsAndOffsets(int kernelSize, double sigma,
                                                              bool forPreprocessorDefine = false,
                                                              bool workaroundForNoCLikeArrayInitialization = false) {
    // Gauss filter kernel & offset creation
    std::vector<double> inputKernel = GenerateSeparableGaussKernel(sigma, kernelSize);

    assert((kernelSize % 2) == 1);
    assert((((kernelSize / 2) + 1) % 2) == 0);

    std::vector<float> oneSideInputs;
    for (int i = (kernelSize / 2); i >= 0; i--) {
        if (i == (kernelSize / 2))
            oneSideInputs.push_back((float) inputKernel[i] * 0.5f);
        else
            oneSideInputs.push_back((float) inputKernel[i]);
    }

    assert((oneSideInputs.size() % 2) == 0);
    int numSamples = oneSideInputs.size() / 2;

    std::vector<float> weights;

    for (int i = 0; i < numSamples; i++) {
        float sum = oneSideInputs[i * 2 + 0] + oneSideInputs[i * 2 + 1];
        weights.push_back(sum);
    }

    std::vector<float> offsets;

    for (int i = 0; i < numSamples; i++) {
        offsets.push_back(i * 2.0f + oneSideInputs[i * 2 + 1] / weights[i]);
    }

    std::string indent = "    ";

    std::string shaderCode = (forPreprocessorDefine) ? ("") : ("");
    std::string eol = (forPreprocessorDefine) ? ("\\\n") : ("\n");
    if (!forPreprocessorDefine)
        shaderCode += indent +
                      "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;" +
                      eol;
    if (!forPreprocessorDefine)
        shaderCode += indent + stringFormatA("// Kernel width %d x %d", kernelSize, kernelSize) + eol;
    if (!forPreprocessorDefine) shaderCode += indent + "//" + eol;
    shaderCode += indent + stringFormatA("const int stepCount = %d;", numSamples) + eol;

    if (!workaroundForNoCLikeArrayInitialization) {
        if (!forPreprocessorDefine) shaderCode += indent + "//" + eol;
        shaderCode += indent + "const float gWeights[stepCount] ={" + eol;
        for (int i = 0; i < numSamples; i++)
            shaderCode +=
                    indent + stringFormatA("   %.5f", weights[i]) + ((i != (numSamples - 1)) ? (",") : ("")) + eol;
        shaderCode += indent + "};" + eol;
        shaderCode += indent + "const float gOffsets[stepCount] ={" + eol;
        for (int i = 0; i < numSamples; i++)
            shaderCode +=
                    indent + stringFormatA("   %.5f", offsets[i]) + ((i != (numSamples - 1)) ? (",") : ("")) + eol;
        shaderCode += indent + "};" + eol;
    }
    else {
        if (!forPreprocessorDefine) shaderCode += indent + "//" + eol;
        shaderCode += indent + "float gWeights[stepCount];" + eol;
        for (int i = 0; i < numSamples; i++)
            shaderCode += indent + stringFormatA(" gWeights[%d] = %.5f;", i, weights[i]) + eol;
        shaderCode += indent + eol;
        shaderCode += indent + "float gOffsets[stepCount];" + eol;
        for (int i = 0; i < numSamples; i++)
            shaderCode += indent + stringFormatA(" gOffsets[%d] = %.5f;", i, offsets[i]) + eol;
        shaderCode += indent + eol;
    }

    if (!forPreprocessorDefine)
        shaderCode += indent +
                      "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////;" +
                      eol;

    return shaderCode;
}

inline std::string GenerateGaussFunctionCode(int kernelSize, double sigma, bool workaroundForNoCLikeArrayInitialization = false) {
    std::string shaderCode;

    std::string eol = "\n";

    shaderCode += eol;
    shaderCode +=
            "// automatically generated by GenerateGaussFunctionCode in GaussianBlur.h                                                                                            " +
            eol;
    shaderCode +=
            "vec3 GaussianBlur( sampler2D tex0, vec2 centreUV, vec2 halfPixelOffset, vec2 pixelOffset )                                                                           " +
            eol;
    shaderCode +=
            "{                                                                                                                                                                    " +
            eol;
    shaderCode +=
            "    vec3 colOut = vec3( 0, 0, 0 );                                                                                                                                   " +
            eol;
    shaderCode +=
            "                                                                                                                                                                     " +
            eol;
    shaderCode += GenerateGaussShaderKernelWeightsAndOffsets(kernelSize, sigma,false,
                                                             workaroundForNoCLikeArrayInitialization);
    shaderCode +=
            "                                                                                                                                                                     " +
            eol;
    shaderCode +=
            "    for( int i = 0; i < stepCount; i++ )                                                                                                                             " +
            eol;
    shaderCode +=
            "    {                                                                                                                                                                " +
            eol;
    shaderCode +=
            "        vec2 texCoordOffset = gOffsets[i] * pixelOffset;                                                                                                           " +
            eol;
    shaderCode +=
            "        vec3 col = texture( tex0, centreUV + texCoordOffset ).xyz + texture( tex0, centreUV - texCoordOffset ).xyz;                                                " +
            eol;
    shaderCode +=
            "        colOut += gWeights[i] * col;                                                                                                                               " +
            eol;
    shaderCode +=
            "    }                                                                                                                                                                " +
            eol;
    shaderCode +=
            "                                                                                                                                                                     " +
            eol;
    shaderCode +=
            "    return colOut;                                                                                                                                                   " +
            eol;
    shaderCode +=
            "}                                                                                                                                                                    " +
            eol;
    shaderCode += eol;

    return shaderCode;
}