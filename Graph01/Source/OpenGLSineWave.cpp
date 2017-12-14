/*
  ==============================================================================

    OpenGLSineWave.cpp
    Created: 11 Dec 2017 12:46:20pm
    Author:  maria kourousi

  ==============================================================================
*/

#include "OpenGLSineWave.h"

OpenGLSineWave::OpenGLSineWave()
{
    // set up OpenGL context and attach it to the component
    openGLContext.setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);

    openGLContext.setContinuousRepainting (true);
}

OpenGLSineWave::~OpenGLSineWave()
{
    // turn off OpenGL
    openGLContext.setContinuousRepainting(false);
    openGLContext.detach();
}

void OpenGLSineWave::newOpenGLContextCreated ()
{
    initResources();
}

void OpenGLSineWave::renderOpenGL ()
{
    jassert(OpenGLHelpers::isContextActive());

    // set up viewport
    const float renderingScale = 1.0f;
    GLsizei width = roundToInt(renderingScale * getWidth());
    GLsizei height = roundToInt(renderingScale * getHeight());
    glViewport(0, 0, width, height);

//    glScissor(border + ticksize, border + ticksize, getWidth() - border * 2 - ticksize, getWidth() - border * 2 - ticksize);
//    glEnable(GL_SCISSOR_TEST);

    // set background colour
    OpenGLHelpers::clear(Colours::darkgrey);

    shaderProgram->use();

    glUniform1f(uniform_transform, 1);

    // Set the color to red
    GLfloat red[4] = { 1, 0, 0, 1 };
    glUniform4fv(uniform_color, 1, red);

    /* Draw using the vertices in our vertex buffer object */
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glEnableVertexAttribArray(attribute_coord2d);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINE_STRIP, 0, 2000);
}

void OpenGLSineWave::openGLContextClosing ()
{

}

void OpenGLSineWave::initResources()
{
    createShaders();

    attribute_coord2d = glGetAttribLocation(shaderProgram->getProgramID(), "coord2d");
    uniform_color = glGetUniformLocation(shaderProgram->getProgramID(), "color");
    uniform_transform = glGetUniformLocation(shaderProgram->getProgramID(), "transform");

//    /* Enable blending */
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Create the vertex buffer object */
    glGenBuffers(3, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    point graph[2000];
    for (int i = 0; i < 2000; ++i)
    {
        float x = (i - 1000.0) / 100.0;
        graph[i].x = x;
        graph[i].y = sin(x * 10.0) / (1.0 + x * x);
    }

    // tell OpenGL to copy our array to the buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);

//    // Enable point size control in vertex shader
//#ifndef GL_ES_VERSION_2_0
//    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//#endif
}

void OpenGLSineWave::createShaders()
{
    vertexShader =
    "attribute vec2 coord2d;\n"
    "uniform mat4 transform;\n"
    "\n"
    "void main()\n"
    "{\n"
        "gl_Position = transform * vec4(coord2d.xy, 0, 1);\n"
    "}\n";

    fragmentShader =
    "uniform vec4 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "       gl_FragColor = color;\n"
    "}\n";

    ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
    String statusText;

    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && newShader->link())
    {
        shaderProgram = newShader;
        shaderProgram->use();
    }
}

