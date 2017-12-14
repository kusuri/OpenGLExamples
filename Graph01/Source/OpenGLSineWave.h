/*
  ==============================================================================

    OpenGLSineWave.h
    Created: 11 Dec 2017 12:46:20pm
    Author:  maria kourousi

  ==============================================================================
*/

#include "JuceHeader.h"

#pragma once

class OpenGLSineWave : public Component,
                        public OpenGLRenderer
{
public:
    struct point {
        GLfloat x;
        GLfloat y;
    };

    
    OpenGLSineWave();
    ~OpenGLSineWave();

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

private:
    void initResources();
    void createShaders();

    OpenGLContext openGLContext;
    ScopedPointer<OpenGLShaderProgram> shaderProgram;

    GLuint VBO[3];
    const char* vertexShader;
    const char* fragmentShader;

    float offset_x = 0.0f;
    float scale_x = 1.0f;
    const int border = 10;
    const int ticksize = 10;

    GLint attribute_coord2d;
    GLint uniform_color;
    GLint uniform_transform;
};
