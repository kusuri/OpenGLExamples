#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void renderFlashingTriangles(unsigned int &program);
unsigned int createShader(const char *shaderSource, bool isVertex);

/*
 The vertex shader is one of the shaders that are programmable by people like us. Modern OpenGL requires that we at least set up a vertex and fragment shader if we want to do some rendering so we will briefly introduce shaders and configure two very simple shaders for drawing our first triangle. In the next tutorial we'll discuss shaders in more detail.
 
 The first thing we need to do is write the vertex shader in the shader language GLSL (OpenGL Shading Language) and then compile this shader so we can use it in our application. 
 */
const char *vertexShaderSource ="#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"}\0";

const char *multiColourVertexShader ="#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

/*
 The fragment shader is the second and final shader we're going to create for rendering a triangle. The fragment shader is all about calculating the color(RGBA) output of your pixels. To keep things simple the fragment shader will always output an orange-ish color.
 */
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";

const char *multiColourFragmentShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(ourColor, 1.0);\n"
"}\0";


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    const GLubyte* v = glGetString (GL_SHADING_LANGUAGE_VERSION);
    std::cout << "OpenGL version:\n" << v << std::endl;
    
    // create shader object
    unsigned int vertexShader = createShader(multiColourVertexShader, true);
    
    // create fragment shader
    unsigned int fragmentShader = createShader(multiColourFragmentShader, false);
    
    // link multiple shaders using a shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int  success;
    char infoLog[512];
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADERPROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
    
    glUseProgram(shaderProgram);
    
    // Oh yeah, and don't forget to delete the shader objects once we've linked them into the program object; we no longer need them anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    // vertex data for drawind a 2D triangle
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions        // colours
        // first triangle
        -0.9f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// top right point
        -0.7f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom right point
        -0.8f,  0.9f, 0.0f, 0.0f, 0.0f, 1.0f,// top left point
        // second triangle
        0.55f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,// top right point
        0.8f, -0.8f, 0.0f, 0.0f, 1.0f, 0.0f,// bottom right point
        0.3f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f// top left point
    };
    
    /* be sure to activate the shader before any calls to glUniform
     Note that finding the uniform location does not require you to use the shader program first,
     but updating a uniform does require you to first use the program (by calling glUseProgram),
     because it sets the uniform on the currently active shader program.
     */
    glUseProgram(shaderProgram);
    
    // manage memory via so called vertex buffer objects (VBO) that can store a large number of vertices in the GPU's memory. we can send large batches of data all at once to the graphics card without having to send data a vertex a time.
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // render loop until the program has been explicitly told to stop from the user
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
     
        // set window colour
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // At the start of each render iteration we always want to clear the screen otherwise we would still see the results from the previous iteration
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//        renderFlashingTriangles(shaderProgram);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window); // swaps the color buffer that has been used to draw in during this iteration and show it as output to the screen
        
        glfwPollEvents();        // checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions (which we can set via callback methods)
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    /* Here we check whether the user has pressed the escape key.
     If the user did press the escape key, we close GLFW by setting its WindowShouldClose property to true 
     using glfwSetwindowShouldClose. 
     The next condition check of the main while loop will then fail and the application closes.
     */
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void renderFlashingTriangles(unsigned int &program)
{
    // update shader uniform
    float timeValue = glfwGetTime(); // get time in seconds
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    int vertexColorLocation = glGetUniformLocation(program, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

unsigned int createShader(const char *shaderSource, bool isVertex)
{
    unsigned int shader;
    if (isVertex)
        shader = glCreateShader(GL_VERTEX_SHADER);
    else
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    // attach the shader source to the shader object and compile the shader
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    // check shader compilation state
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    return shader;
}