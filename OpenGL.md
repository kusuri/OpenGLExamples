# OpenGL
- graphics API
- not a library, it's a specification
- cross platform 
- state machine: a collection of variables that define how OpenGL should currently
operate
- works in 3D space

## Objects

## Vertex buffer
A block of memory containing the data and stored in GPU.

In order to draw and/or animate a 3-D object, points have to be defined in the
3D world to define a "skeleton" representing the object to be drawn/animated.
The skeleton is defined using polygons (generally triangles or quadrilaterals)
whose angular points are called vertices. For example a cube defined using
triangles can be drawn using 6 faces x 2 triangles x 3 vertices = 36 vertices
(in fact some of the vertices are shared by different faces and in some instances
can therefore be defined as one vertex).

## VBO VS VAO VS EBO
http://antongerdelan.net/opengl/vertexbuffers.html

- **V**ertex **B**uffer **O**bject: array of data (usually floats)
- **V**ertex **A**rray **O**bject: tell OpenGL that the array is divided into variables of 3 (x, y, z) floats each

## Fragment
A fragment is a surface linking 2 or more vertices. It is the "skin" covering the
 "skeleton" representing a 3-D object.
 
## Shader
GLSL language: small routines(shaders) that executed directly by the GPU

### GLSL
Shaders are written in the C-like language GLSL. Vector and matrix manipulation.

#### Structure
- version
- list of input and output variables (**in**, **out** keywords)
- uniforms : 
    - uniforms are a useful tool for setting attributes that might change in render iterations, or for interchanging data between your application and your shaders
    - pass data from our application on the CPU to the shaders on the GPU,
    - global, unique per shader program object, 
    - can be accessed from any shader at any stage in the shader program
    - whatever you set the uniform value to, uniforms will keep their values until they're either reset or updated
    - If you declare a uniform that isn't used anywhere in your GLSL code the compiler will silently remove the variable from the compiled version which is the cause for several frustrating errors; keep this in mind!
- main function

#### Supported types
- int, **float**, double, uint, bool
- vectors and matrices (e.g vec4)

### Vertex shaders
The Vertex Shader defines the actions executed by the GPU to define vertex attributes (e.g. each vertex coordinates, color, etc.). 
**Vertex attribute**: input variables of vertex shader

The vertex shader should receive some form of input otherwise it would be pretty ineffective. The vertex shader differs in its input, in that it receives its input straight from the vertex data. 

### Fragment shader
The Fragment Shader, which is executed after the Vertex Shader, defines the
attributes of all the pixels other than those corresponding to the vertices.
In other words, it will render (paint) the texture interpolated between vertices.
Since the GPU essentially is a state machine (upon each new state
a new frame is drawn on the screen), the Vertex Shader defines the attributes
of each vertex to be drawn upon each state, and passes the information to the
Fragment shader which may use it to render the texture upon each frame.
Note that each shader includes not only instructions but also data that can
be shared between the main CPU code and between the shaders themselves.
So basically upon each state (frame) the CPU will send data (and eventually
new instructions) to the GPU by using either OpenGL instructions or by sending
new data to be used by the GPU shaders.

The fragment shader requires a vec4 color output variable, since the fragment shaders needs to generate a final output color.

**IMPORTANT:** Both instructions and data are defined in source form, and therefore
need to be compiled and linked ,like any computer program, to be executed by
the GPU. But the source code is compiled by the OpenGL library itself (by
calling some specific methods) AT RUN TIME.

### Vertex shader VS fragment shader
A fragment shader is the same as pixel shader.

One main difference is that a vertex shader can manipulate the attributes of vertices. which are the corner points of your polygons.

The fragment shader on the other hand takes care of how the pixels between the vertices look. They are interpolated between the defined vertices following specific rules.

For example: if you want your polygon to be completely red, you would define all vertices red. If you want for specific effects like a gradient between the vertices, you have to do that in the fragment shader.

Put another way:

The vertex shader is part of the early steps in the graphic pipeline, somewhere between model coordinate transformation and polygon clipping I think. At that point, nothing is really done yet.

However, the fragment/pixel shader is part of the rasterization step, where the image is calculated and the pixels between the vertices are filled in or "coloured".


## OpenGL and Juce

### Juce classes
- OpenGLContext: Creates an OpenGL context, which can be attached to a component. An OpenGL context represents many things. Think of a context as an object that holds all of OpenGL; when a context is destroyed, OpenGL is destroyed.
- OpenGLHelpers:
- OpenGLFrameBuffer:
- OpenGLTexture:
- OpenGLImageType:
- OpenGLPixelFormat:
- OpenGLGraphicsContextCustomShader:
- 
- OpenGLRenderer: A base class that should be implemented by classes which want to render openGL on a background thread.
- OpenGLAppComponent: A base class for writing simple one-page graphical apps.
- OpenGLShaderProgram: Manages an OpenGL shader program.



## GLWL 
OSX installation

- download [cmake](https://cmake.org/download/)  and [glwl](http://www.glfw.org/download.html)
- open terminal 
	- cd <directory of cmake>
	- sudo bootstrap
	- sudo make
	- sudo makeinstall
- cd <glwl directory> 
- unzip glfw-3.0.1.zip
- mkdir build
- cd build
- export MACOSX_DEPLOYMENT_TARGET=10.<osx version>
- cmake -D GLFW_NATIVE_API=1 -D CMAKE_OSX_ARCHITECTURES="i386;x86_64" -D BUILD_SHARED_LIBS=ON -D CMAKE_C_COMPILER=clang ../
- make


