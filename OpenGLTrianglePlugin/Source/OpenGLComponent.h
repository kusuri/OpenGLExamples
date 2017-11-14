/*
  ==============================================================================

    OpenGLComponent.h
    Created: 14 Nov 2017 11:30:59am
    Author:  maria kourousi

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


struct Vertex  // class storing the information about a single vertex
{
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
};

class OpenGLComponent : public OpenGLAppComponent, private Timer
{
public:
    OpenGLComponent();
    ~OpenGLComponent();
    
    void initialise() override;
    void shutdown() override;
    void render() override;
    void paint (Graphics& g) override;
    void resized() override;
    void createShaders();
    void timerCallback() override;
    
private:
    GLuint vertexBuffer, indexBuffer;
    int numIndices;
    
    float x;
    float y;
    
    const char* vertexShader;
    const char* fragmentShader;
    
    ScopedPointer<OpenGLShaderProgram> shader;
    ScopedPointer<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;
    
    String newVertexShader, newFragmentShader;
};