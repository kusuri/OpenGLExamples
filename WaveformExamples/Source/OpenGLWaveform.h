/*
  ==============================================================================

    OpenGLWaveform.h
    Created: 6 Dec 2017 3:29:17pm
    Author:  maria kourousi

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "RingBuffer.h"

#define RING_BUFFER_READ_SIZE 512

class OpenGLWaveform : public Component,
                        public OpenGLRenderer
{
public:
    OpenGLWaveform(RingBuffer<GLfloat> * ringBuffer, int size);
    ~OpenGLWaveform();

    /** Overriden method of OpenGLRenderer class.

     Called before rendering OpenGL, after an OpenGLContext has been associated
     with this OpenGlRenderer (this component is an OpenGLRenderer).
     Sets up GL objects that are needed for rendering.
     */
    void newOpenGLContextCreated () override;

    /** Overriden method of OpenGLRenderer class.


    */
    void renderOpenGL () override;

    /** Overriden method of OpenGLRenderer class.

     Called when done rendering OpenGL, as an OepnGLContext object is closing.
     Free any GL objects during rendering.
     */
    void openGLContextClosing () override;

    void start();
    void stop();
private:
    void initVertexObjects();

    /** Loads the OpenGL Shaders and sets up the whole ShaderProgram
     */
    void createShaders();

//==============================================================================
    // This class just manages the uniform values that the fragment shader uses.
    struct Uniforms
    {
        Uniforms (OpenGLContext& openGLContext, OpenGLShaderProgram& shaderProgram)
        {
            resolution          = createUniform (openGLContext, shaderProgram, "resolution");
            audioSampleData     = createUniform (openGLContext, shaderProgram, "audioSampleData");
        }

        ScopedPointer<OpenGLShaderProgram::Uniform> resolution, audioSampleData;
    private:
        static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& openGLContext,
                                                            OpenGLShaderProgram& shaderProgram,
                                                            const char* uniformName)
        {
            if (openGLContext.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName) < 0)
                return nullptr;

            return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName);
        }
    };

//==============================================================================
    // OpenGL variables
    OpenGLContext openGLContext; // [1] Create an OpenGL context. Think of a context as an object that holds all of OpenGL; when a context is destroyed, OpenGL is destroyed.
    GLuint VBO, VAO, EBO; // [2] Create vertex, array, element buffer objects
    ScopedPointer<OpenGLShaderProgram> shaderProgram; // [3] Create an OpenGL shader program
    ScopedPointer<Uniforms> uniforms;
    const char* vertexShader; // [4] ??
    const char* fragmentShader;

    // Audio Buffer
    int bufferSize;
    RingBuffer<GLfloat> * ringBuffer;
    AudioBuffer<GLfloat> readBuffer;    // Stores data read from ring buffer
    GLfloat visualizationBuffer[RING_BUFFER_READ_SIZE];    // Single channel to visualize
};

