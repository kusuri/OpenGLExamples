/*
  ==============================================================================

    OpenGLComponent.cpp
    Created: 14 Nov 2017 11:30:59am
    Author:  maria kourousi

  ==============================================================================
*/

#include "OpenGLComponent.h"


OpenGLComponent::OpenGLComponent()
{
    // Sets the OpenGL version to 3.2
    openGLContext.setOpenGLVersionRequired (OpenGLContext::OpenGLVersion::openGL3_2);
    
    // Attach the OpenGL context but do not start [ see start() ]
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting (true);
    
//    setSize (800, 600);
    startTimerHz(1000);
    x = 0;
    y = 0;
    position = nullptr;
    normal = nullptr;
    textureCoordIn = nullptr;
    sourceColour = nullptr;
}

OpenGLComponent::~OpenGLComponent()
{
    // Turn off OpenGL
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

//==========================================================================
// OpenGL Callbacks

/** Called before rendering OpenGL, after an OpenGLContext has been associated
 with this OpenGLRenderer (this component is a OpenGLRenderer).
 Sets up GL objects that are needed for rendering.
 */
void OpenGLComponent::newOpenGLContextCreated()
{
    // Setup Shaders
    createShaders();
    
    // Setup Buffer Objects
//    openGLContext.extensions.glGenBuffers (1, &VBO); // Vertex Buffer Object
//    openGLContext.extensions.glGenBuffers (1, &EBO); // Element Buffer Object
}

/** Called when done rendering OpenGL, as an OpenGLContext object is closing.
 Frees any GL objects created during rendering.
 */
void OpenGLComponent::openGLContextClosing()
{
    shader->release();
    shader = nullptr;
}

void OpenGLComponent::renderOpenGL()
{
    // Stuff to be done before defining your triangles
    jassert (OpenGLHelpers::isContextActive());
    
    const float desktopScale = (float) openGLContext.getRenderingScale();
    OpenGLHelpers::clear (Colour(45, 69, 68));
    
    //        glEnable (GL_BLEND);
    //        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //        glViewport (0, 0, roundToInt (desktopScale * getWidth()), roundToInt (desktopScale * getHeight()));
    
    shader->use();
    
    openGLContext.extensions.glGenBuffers (1, &vertexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
    
    
    // ************************** TRIANGLES DEFINITION
    // Here you can draw whatever you want
    
    Array<Vertex> vertices;
    Vertex v1 =
    {
        { -0.9f + x, 0.5f + y, 0.0f},
        { 0.5f + x, 0.5f + y, 0.5f},
        { 1.0f + x, 0.0f + y, 0.0f, 1.0f },
        { 0.5f, 0.5f,}
    };
    vertices.add (v1);
    Vertex v2 =
    {
        { -0.7f + x, 0.5f + y, 0.0f},
        { 0.5f + x, 0.5f + y, 0.5f},
        { 0.0f + x, 1.0f + y, 0.0f, 1.0f },
        { 0.5f, 0.5f}
    };
    vertices.add (v2);
    Vertex v3 =
    {
        { -0.8f + x,  0.9f + y, 0.0f},
        { 0.5f + x, 0.5f + y, 0.5f},
        { 0.0f + x, 0.0f + y, 1.0f, 1.0f },
        { 0.5f, 0.5f,}
    };
    vertices.add (v3);
    
    Vertex v4 =
    {
        { 0.55f + x, -0.5f + y, 0.0f},
        { 0.5f + x, 0.5f + y, 0.5f},
        { 1.0f + x, 0.0f + y, 0.0f, 1.0f },
        { 0.5f, 0.5f,}
    };
    vertices.add (v4);
    Vertex v5 =
    {
        { 0.8f + x, -0.8f + y, 0.0f},
        { 0.5f + x, 0.5f + y, 0.5f},
        { 0.0f + x, 1.0f + y, 0.0f, 1.0f },
        { 0.5f, 0.5f}
    };
    vertices.add (v5);
    Vertex v6 =
    {
        { 0.3f + x, -0.8f, 0.0f},
        { 0.5f + x, 0.5f, 0.5f},
        { 0.0f + x, 0.0f, 1.0f, 1.0f },
        { 0.5f, 0.5f,}
    };
    vertices.add (v6);
    int indices[] = {0, 1, 2, 3, 4, 5};
    numIndices = 6;
    // ************************************************
    
    // Now prepare this information to be drawn
    openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                           static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                           vertices.getRawDataPointer(), GL_STATIC_DRAW);
    
    openGLContext.extensions.glGenBuffers (1, &indexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                           static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                                           indices, GL_STATIC_DRAW);
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    if (position != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
        openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
    }
    
    if (normal != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
        openGLContext.extensions.glEnableVertexAttribArray (normal->attributeID);
    }
    
    if (sourceColour != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
        openGLContext.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
    }
    
    if (textureCoordIn != nullptr)
    {
        openGLContext.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
        openGLContext.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
    }
    
    //glPointSize(20.0);
    //        glDrawElements (GL_POINTS, numIndices, GL_UNSIGNED_INT, 0);  // Draw triangles!
    glDrawElements (GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);  // Draw triangles!
    
    if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
    if (normal != nullptr)         openGLContext.extensions.glDisableVertexAttribArray (normal->attributeID);
    if (sourceColour != nullptr)   openGLContext.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
    if (textureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
    
    // Reset the element buffers so child Components draw correctly
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    
    openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
    openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
    
}

void OpenGLComponent::paint (Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
}

void OpenGLComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
//    auto rect = getLocalBounds();
//    slider->setBounds (rect.removeFromRight(100). withSize(100, 100));
    
}

void OpenGLComponent::createShaders()
{
    // Here we define the shaders use to draw our triangle. They are very simple.
    
    vertexShader =
    "attribute vec4 position;\n"
    "attribute vec4 sourceColour;\n"
    "attribute vec2 textureCoordIn;\n"
    "\n"
    "varying vec4 destinationColour;\n"
    "varying vec2 textureCoordOut;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    destinationColour = sourceColour;\n"
    "    textureCoordOut = textureCoordIn;\n"
    "    gl_Position = position;\n"
    "}\n";
    
    fragmentShader =
#if JUCE_OPENGL_ES
    "varying lowp vec4 destinationColour;\n"
    "varying lowp vec2 textureCoordOut;\n"
#else
    "varying vec4 destinationColour;\n"
    "varying vec2 textureCoordOut;\n"
#endif
    "\n"
    "uniform sampler2D demoTexture;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = destinationColour; \n"
    "}\n";
    
    ScopedPointer<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));
    double v = newShader->getLanguageVersion();
    DBG("OpenGL version: " + String(v));
    String statusText;
    
    if (newShader->addVertexShader (OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && newShader->addFragmentShader (OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && newShader->link())
    {
        shader = newShader;
        shader->use();
        
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "position") < 0)
            position      = nullptr;
        else
            position      = new OpenGLShaderProgram::Attribute (*shader,    "position");
        
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "sourceColour") < 0)
            sourceColour      = nullptr;
        else
            sourceColour  = new OpenGLShaderProgram::Attribute (*shader,    "sourceColour");
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "normal") < 0)
            normal      = nullptr;
        else
            normal      = new OpenGLShaderProgram::Attribute (*shader,    "normal");
        if (openGLContext.extensions.glGetAttribLocation (shader->getProgramID(), "textureCoordIn") < 0)
            textureCoordIn      = nullptr;
        else
            textureCoordIn      = new OpenGLShaderProgram::Attribute (*shader,    "textureCoordIn");
        
        statusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        statusText = newShader->getLastError();
    }
}

void OpenGLComponent::timerCallback()
{
    // Here you move your triangles
    x = x + 0.001;
    y = y + 0.001;
    if (x > 1)
        x -= 2;
    if (y > 1)
        y -= 2;
}