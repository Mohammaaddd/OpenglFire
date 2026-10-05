#include<glad/glad.h>
#include <GLFW/glfw3.h>

float vertices[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.5f
};

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos.x * 0.5f, aPos.y * 2.0f, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.3, 0.05, 1.0);
}
)";

unsigned int createShaderProgram() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main()
{
    //init glfw
    glfwInit();

    //tell opengl what version we are gonna use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //create the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "openGL Fire", NULL, NULL);

    //this is the window we're currently rendering into
    glfwMakeContextCurrent(window);

    //init glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //vertex array object
    unsigned int VAO;

    //vertex buffer object
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    //upload vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int shaderProgram = createShaderProgram();

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //enables the vertex attribute at location 0
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window)) {

        //when clearing the screen use this color
        glClearColor(0.1f, 0.05f, 0.02f, 1.0f);

        //clear the screen using the color we chose
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        glfwSwapBuffers(window);


        glfwPollEvents();
    }
}
    //Vertex shader : processes each vertex and determines its position.
    //Fragment shader : determines the color of the pixels covered by our triangle.