/*
  ==============================================================================

    OpenGLWaveform.cpp
    Created: 6 Dec 2017 3:29:17pm
    Author:  maria kourousi

  ==============================================================================
*/

#include "OpenGLWaveform.h"

OpenGLWaveform::OpenGLWaveform(RingBuffer<GLfloat> * ringBuffer, int size)
: bufferSize(size), readBuffer (2, RING_BUFFER_READ_SIZE)
{
//    visualizationBuffer = new GLfloat[bufferSize];

    this->ringBuffer = ringBuffer;

    // set up OpenGL context and attach it to the component
    openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
}

OpenGLWaveform::~OpenGLWaveform()
{
    // turn off OpenGL
    openGLContext.setContinuousRepainting(false);
    openGLContext.detach();

    // Detach ringBuffer
    ringBuffer = nullptr;
}

void OpenGLWaveform::newOpenGLContextCreated()
{
     createShaders();

    // set up OpenGL buffer objects
    openGLContext.extensions.glGenBuffers(1, &VBO);
    openGLContext.extensions.glGenBuffers(1, &EBO);
}

void OpenGLWaveform::renderOpenGL()
{
    jassert(OpenGLHelpers::isContextActive());

    // set up viewport
    const float renderingScale = /*(float) openGLContext.getRenderingScale()*/0.5;
    GLsizei width = roundToInt(renderingScale * getWidth());
    GLsizei height = roundToInt(renderingScale * getHeight());
    glViewport(0, 0, width, height);

    // set background colour
    OpenGLHelpers::clear(Colours::darkgrey);

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // use defined shader program
    shaderProgram->use();

    // set up the uniforms for use in the shader
    if (uniforms->resolution != nullptr)
    {
        uniforms->resolution->set((GLfloat) renderingScale * getWidth(), (GLfloat) renderingScale*getHeight());
    }

    // read in samples from ring buffer
    if (uniforms->audioSampleData != nullptr)
    {
        ringBuffer->readSamples (readBuffer, RING_BUFFER_READ_SIZE);

        FloatVectorOperations::clear (visualizationBuffer, RING_BUFFER_READ_SIZE);

        // Sum channels together
        for (int i = 0; i < 2; ++i)
        {
            FloatVectorOperations::add (visualizationBuffer, readBuffer.getReadPointer(i, 0), RING_BUFFER_READ_SIZE);
        }

        uniforms->audioSampleData->set(visualizationBuffer, RING_BUFFER_READ_SIZE);
    }

    initVertexObjects();

    // set up vertex attributes
    openGLContext.extensions.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    openGLContext.extensions.glEnableVertexAttribArray(0);

    // draw vertices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // reset the element buffers so child components draw correctly
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLWaveform::openGLContextClosing()
{
    shaderProgram->release();
    shaderProgram = nullptr;
    uniforms = nullptr;
}

void OpenGLWaveform::initVertexObjects()
{
    // Define Vertices for a Square (the view plane)
    GLfloat vertices[] = {
        1.0f,   1.0f,  0.0f,  // Top Right
        1.0f,  -1.0f,  0.0f,  // Bottom Right
        -1.0f, -1.0f,  0.0f,  // Bottom Left
        -1.0f,  1.0f,  0.0f   // Top Left
    };
    // Define Which Vertex Indexes Make the Square
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };

    // VBO - Bind and write to buffer
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW); // GL_STREAM_DRAW or GL_DYNAMIC_DRAW since the vertex data will be changing a lot?

    // EBO - bind and write to buffer
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
}

/** Loads the OpenGL Shaders and sets up the whole ShaderProgram
 */
void OpenGLWaveform::createShaders()
{
    vertexShader =
    "attribute vec3 position;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position, 1.0);\n"
    "}\n";

    fragmentShader =
    "uniform vec2  resolution;\n"
    "uniform float audioSampleData[512];\n"
    "\n"
    "void getAmplitudeForXPos (in float xPos, out float audioAmplitude)\n"
    "{\n"
    // Buffer size - 1
    "   float perfectSamplePosition = 255.0 * xPos / resolution.x;\n"
    "   int leftSampleIndex = int (floor (perfectSamplePosition));\n"
    "   int rightSampleIndex = int (ceil (perfectSamplePosition));\n"
    // OpenGL: mix — linearly interpolate between two values
    "   audioAmplitude = mix (audioSampleData[leftSampleIndex], audioSampleData[rightSampleIndex], fract (perfectSamplePosition));\n"
    "}\n"
    "\n"
    "#define THICKNESS 0.01\n"
    "void main()\n"
    "{\n"
    "    float y = gl_FragCoord.y / resolution.y;\n"
    "    float amplitude = 0.0;\n"
    "    getAmplitudeForXPos (gl_FragCoord.x, amplitude);\n"
    "\n"
    // Centers & Reduces Wave Amplitude
    "    amplitude = 0.5 - amplitude / 2.5;\n"
    "    float r = abs (THICKNESS / (amplitude-y));\n"
    "\n"
    "gl_FragColor = vec4 (r - abs (r * 0.2), r - abs (r * 0.2), r - abs (r * 0.2), 1.0);\n"
    "}\n";

    ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
    String statusText;

    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && newShader->link())
    {
        uniforms = nullptr;

        shaderProgram = newShader;
        shaderProgram->use();

        uniforms   = new Uniforms (openGLContext, *shaderProgram);

    }
}

void OpenGLWaveform::start()
{
    openGLContext.setContinuousRepainting (true);
}

void OpenGLWaveform::stop()
{
    openGLContext.setContinuousRepainting (false);
}

