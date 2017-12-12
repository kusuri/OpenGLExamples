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

    // set background colour
    OpenGLHelpers::clear(Colours::darkgrey);

    shaderProgram->use();

    glUniform1i(uniform_mytexture, 0);
    glUniform1f(uniform_offset_x, offset_x);
    glUniform1f(uniform_scale_x, scale_x);


    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw using the vertices in our vertex buffer object */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(attribute_coord1d);
    glVertexAttribPointer(attribute_coord1d, 1, GL_FLOAT, GL_FALSE, 0, 0);

//    glUniform1f(uniform_sprite, 0);
    glDrawArrays(GL_LINE_STRIP, 0, 101);
}

void OpenGLSineWave::openGLContextClosing ()
{

}

void OpenGLSineWave::initResources()
{
    createShaders();

    attribute_coord1d = glGetAttribLocation(shaderProgram->getProgramID(), "coord1d");
    uniform_offset_x = glGetUniformLocation(shaderProgram->getProgramID(), "offset_x");
    uniform_scale_x = glGetUniformLocation(shaderProgram->getProgramID(), "scale_x");
//    uniform_sprite = glGetUniformLocation(shaderProgram->getProgramID(), "sprite");
    uniform_mytexture = glGetUniformLocation(shaderProgram->getProgramID(), "mytexture");

    /* Enable blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    /* Upload the texture for our point sprites */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 2048, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, graph);

    /* Create the vertex buffer object */
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Create an array with only x values
    GLfloat line[101];
    for (int iSample = 0; iSample < 101; ++iSample)
    {
        line[iSample] = (iSample - 50.0f) / 50.0f;
    }

    // tell OpenGL to copy our array to the buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    // Enable point size control in vertex shader
#ifndef GL_ES_VERSION_2_0
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
}

void OpenGLSineWave::createShaders()
{
    vertexShader =
    "attribute vec2 coord1d;\n"
    "varying vec4 f_color;\n"
    "uniform float offset_x;\n"
    "uniform float scale_x;\n"
    "uniform lowp float sprite;\n"
    "\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4((coord1d.x + offset_x) * scale_x, coord1d.y, 0, 1);\n"
    "f_color = vec4(coord1d.xy/2.0 + 0.5, 1, 1);\n"
    "gl_PointSize = max(1.0, sprite);\n"
    "}\n";

    fragmentShader =
    "uniform sampler2D mytexture;\n"
    "varying vec4 f_color;\n"
    "uniform float sprite;\n"
    "\n"
    "void main()\n"
    "{\n"
    "       gl_FragColor = f_color;\n"
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

