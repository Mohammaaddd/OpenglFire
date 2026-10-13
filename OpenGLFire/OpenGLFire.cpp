#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <cmath>
#include <random>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

float vertices[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,

    -0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

std::string readShaderFile(const char* filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << '\n';

        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "Failed to load shader source. \n";
        return 0;
    }

    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();


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

float windStrength = 0.0f;

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
std::uniform_real_distribution<float> turbulence(-0.4f, 0.4f);

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

    const int particleCount = 2000;

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

    unsigned int shaderProgram = createShaderProgram("particle.vert", "particle.frag");
    unsigned int flameShaderProgram = createShaderProgram("flame.vert", "flame.frag");

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //enables the vertex attribute at location 0
    glEnableVertexAttribArray(0);

    //this is the uniform
    int particlePositionLocation = glGetUniformLocation(shaderProgram, "particlePosition");
    int particleSizeLocation = glGetUniformLocation(shaderProgram, "particleSize");
    int particleColorLocation = glGetUniformLocation(shaderProgram, "particleColor");
    int particleHeightLocaiton = glGetUniformLocation(shaderProgram, "particleHeight");
    int particleFadeLocation = glGetUniformLocation(shaderProgram, "particleFade");
    int particleFlickerLocation = glGetUniformLocation(shaderProgram, "particleFlicker");

    //flame
    int timeLocation = glGetUniformLocation(flameShaderProgram, "time");

    float lastFrame = 0.0f;
    float triangleX = 0.0f;
    float fadeDuration = 0.7f;



    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();

        float deltaTime = currentFrame - lastFrame;

        windStrength = std::sin(glfwGetTime() * 0.7f) * 0.15f;

        lastFrame = currentFrame;

        //triangleX = std::sin(currentFrame * 3.0f) * 0.5f;

        // Update particles
        for (Particle& particle : particles)
        {
            //particle.position += particle.velocity * deltaTime;

            particle.lifetime -= deltaTime;

            float height = particle.position.y + 0.5f;

            float spread = height * 0.08f;

            particle.position.x += (turbulence(randomGenerator) * 0.5f + windStrength) * deltaTime;

            particle.position.x += particle.velocity.x * deltaTime
                + (particle.position.x >= 0.0f ? spread : -spread) * deltaTime;

            particle.velocity.y += 0.3f * deltaTime;

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
        
        //glBindVertexArray(VAO);

        // Draw particles
        for (const Particle& particle : particles) {
            float heightFactor = glm::clamp((particle.position.y + 0.5f) / 1.0f, 0.0f, 1.0f);

            float currentSize = particle.size * (1.0f - heightFactor * 0.6f);

            float particleFade = glm::clamp(particle.lifetime / fadeDuration, 0.0f, 1.0f);

            float particleFlicker = 0.85f + 0.15f * std::sin(static_cast<float>(glfwGetTime()) * 12.0f + particle.position.x * 20.0f + particle.position.y * 15.0f);

            //here we add the uniform
            glUniform1f(particleSizeLocation, currentSize);
            glUniform1f(particleFadeLocation, particleFade);
            glUniform1f(particleFlickerLocation, particleFlicker);
            glUniform1f(particleHeightLocaiton, glm::clamp((particle.position.y + 0.5f) / 1.0f, 0.0f, 1.0f));
            glUniform2f(particlePositionLocation, particle.position.x, particle.position.y);
            glUniform3f(particleColorLocation, particle.color.x, particle.color.y, particle.color.z);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        float currentTime = static_cast<float>(glfwGetTime());
        glUniform1f(timeLocation, currentTime * 10);

        glUseProgram(flameShaderProgram);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);


        glfwPollEvents();
    }
}
    //Vertex shader : processes each vertex and determines its position.
    //Fragment shader : determines the color of the pixels covered by our triangle.
    //A uniform allows our C++ program to send a value to a shader.








    //old code
/*float glow = 1.0 - (distanceFromCenter / 0.5);
glow = glow * glow;
glow = smoothstep(0.0, 1.0, glow);*/