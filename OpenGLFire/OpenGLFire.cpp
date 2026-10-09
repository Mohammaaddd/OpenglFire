#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <cmath>
#include <random>
#include <vector>
//#include <iostream>

float vertices[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,

    -0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 particlePosition;
uniform float particleSize;

out vec2 localPosition;

void main()
{
    localPosition = aPos;

    gl_Position = vec4(aPos * particleSize + particlePosition, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

uniform vec3 particleColor;

in vec2 localPosition;

void main()
{
    float distanceFromCenter = length(localPosition);

    //this the code that remove the edges and make it a circle
    if(distanceFromCenter > 0.5)
    {
        discard;
    }

    float glow = 1.0 - (distanceFromCenter / 0.5);

    glow = smoothstep(0.0, 1.0, glow);

    FragColor = vec4(particleColor, glow);
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

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;

    float lifetime;
    float size;

    glm::vec3 color;
};

std::random_device randomDevice;
std::mt19937 randomGenerator(randomDevice());
std::uniform_real_distribution<float> horizontalVelocity(-0.2f, 0.2f);
std::uniform_real_distribution<float> verticalVelocity(0.4f, 1.0f);
std::uniform_real_distribution<float> particleLifetime(2.0f, 5.0f);
std::uniform_real_distribution<float> particleSize(0.03f, 0.12f);
std::uniform_real_distribution<float> particleRed(0.8f, 1.0f);
std::uniform_real_distribution<float> particleGreen(0.1f, 0.45f);
std::uniform_real_distribution<float> particleBlue(0.0f, 0.08f);
std::uniform_real_distribution<float> spawnPositionX(-0.15f, 0.15f);

void respawnParticle(Particle& particle)
{
    particle.position = glm::vec2(spawnPositionX(randomGenerator), -0.5f);

    particle.velocity = glm::vec2(horizontalVelocity(randomGenerator), verticalVelocity(randomGenerator));

    particle.lifetime = particleLifetime(randomGenerator);
    particle.size = particleSize(randomGenerator);
    particle.color = glm::vec3(particleRed(randomGenerator), particleGreen(randomGenerator), particleBlue(randomGenerator));
}

int main()
{
    //init glfw
    glfwInit();

    //std::cout << sizeof(Particle);

    std::vector<Particle> particles;

    const int particleCount = 50;

    for (int i = 0; i < particleCount; ++i)
    {
        Particle particle;

        respawnParticle(particle);

        particles.push_back(particle);
    }

    /*particle.position = glm::vec2(0.0f, -0.5f);
    particle.velocity = glm::vec2(horizontalVelocity(randomGenerator), 0.5f);
    particle.lifetime = 5.0f;
    particle.size = 0.1f;*/

    //tell opengl what version we are gonna use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //create the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "openGL Fire", NULL, NULL);

    //this is the window we're currently rendering into
    glfwMakeContextCurrent(window);

    //init glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    //enables blending, allowing the fragment's alpha value to affect how it combines with the existing screen color.
    glEnable(GL_BLEND);

    //chooses how the new color combines with the background. This is standard alpha blending, which lets transparent particle edges fade smoothly into the scene.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    //this is the uniform
    int particlePositonLocation = glGetUniformLocation(shaderProgram, "particlePosition");
    int particleSizeLocation = glGetUniformLocation(shaderProgram, "particleSize");
    int particleColorLocation = glGetUniformLocation(shaderProgram, "particleColor");

    float lastFrame = 0.0f;
    float triangleX = 0.0f;



    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();

        float deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;

        //triangleX = std::sin(currentFrame * 3.0f) * 0.5f;

        // Update particles
        for (Particle& particle : particles)
        {
            //particle.position += particle.velocity * deltaTime;

            particle.lifetime -= deltaTime;

            float height = particle.position.y + 0.5f;

            float spread = height * 0.15f;

            particle.position.x += particle.velocity.x * deltaTime
                + (particle.position.x >= 0.0f ? spread : -spread) * deltaTime;

            particle.position.y += particle.velocity.y * deltaTime;

            if (particle.lifetime <= 0.0f)
            {
                respawnParticle(particle);
            }
        }




        //--------------RENDERING--------------

        //when clearing the screen use this color
        glClearColor(0.1f, 0.05f, 0.02f, 1.0f);

        //clear the screen using the color we chose
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        glBindVertexArray(VAO);

        // Draw particles
        for (const Particle& particle : particles) {
            //here we add the uniform
            glUniform1f(particleSizeLocation, particle.size);
            glUniform2f(particlePositonLocation, particle.position.x, particle.position.y);
            glUniform3f(particleColorLocation, particle.color.x, particle.color.y, particle.color.z);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }



        glfwSwapBuffers(window);


        glfwPollEvents();
    }
}
    //Vertex shader : processes each vertex and determines its position.
    //Fragment shader : determines the color of the pixels covered by our triangle.
    //A uniform allows our C++ program to send a value to a shader.