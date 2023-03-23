#include "rendering.h"
#include "stb_image.h"

#include "glad/glad.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

Renderer::Renderer(unsigned int resx, unsigned int resy) : screenResX(resx), screenResY(resy)
{

}

Renderer::~Renderer()
{
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
}

void Renderer::loadImage(const char* data, unsigned int& texture, int& width, int& height, bool flip)
{
    int nrChannels;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* fileData = stbi_load(data, &width, &height, &nrChannels, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fileData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(fileData);
}

// load all the image resource files
void Renderer::initializeData()
{
    // player images
    int width, height;
    loadImage("..\\Resources\\Sprites\\bluebird-downflap.png", playerTexture1, width, height, true);
    loadImage("..\\Resources\\Sprites\\bluebird-midflap.png", playerTexture2, width, height, true);
    loadImage("..\\Resources\\Sprites\\bluebird-upflap.png", playerTexture3, width, height, true);

    // these vertices are in "local space"
    float playerVertices[] = {
        width * 0.5f, height * 0.5f, 0.0f,   1.0f, 1.0f, // top right
        width * 0.5f, height * -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        width * -0.5f,  height * 0.5f, 0.0f,  0.0f, 1.0f,  // top left
        width * 0.5f, height * -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        width * -0.5f, height * -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        width * -0.5f,  height * 0.5f, 0.0f,  0.0f, 1.0f  // top left
    };

    // generate other player buffers
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    playerProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");

    // tube
    loadImage("..\\Resources\\Sprites\\pipe-green.png", tubeTexture, width, height, true);

    // these vertices are in "local space"
    float tubeVertices[] = {
        width * 0.5f, height * 0.5f, 0.0f,   1.0f, 1.0f, // top right
        width * 0.5f, height * -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        width * -0.5f,  height * 0.5f, 0.0f,  0.0f, 1.0f,  // top left
        width * 0.5f, height * -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        width * -0.5f, height * -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        width * -0.5f,  height * 0.5f, 0.0f,  0.0f, 1.0f  // top left
    };

    glGenVertexArrays(1, &tubeVAO);
    glGenBuffers(1, &tubeVBO);
    glBindVertexArray(tubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tubeVertices), tubeVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    tubeProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");

    // background
    loadImage("..\\Resources\\Sprites\\background-day.png", backgroundTexture, width, height, true);

    float backgroundVertices[] = {
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    backgroundProgram = shaderStuff("..\\Shaders\\texture.vert", "..\\Shaders\\texture.frag");
}

void Renderer::drawPlayer(GameObject& player, const std::chrono::microseconds& deltaTime)
{
    // need to handle switching between frames just use delta time probably
    // player matrix transforms - this is where we set the player position and size
    static std::chrono::microseconds timeCollector = std::chrono::microseconds(0);
    static unsigned int currentTexture = playerTexture1;
    timeCollector += deltaTime;
    if (timeCollector > std::chrono::microseconds(112000))
    {
        timeCollector = std::chrono::microseconds(0);
        if (currentTexture == playerTexture1)
            currentTexture = playerTexture2;
        else if (currentTexture == playerTexture2)
            currentTexture = playerTexture3;
        else
            currentTexture = playerTexture1;
    }

    glUseProgram(playerProgram);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(player.positionX / screenResX - 1.0f, player.positionY / screenResY - 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(player.rotation), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(player.scaleX / screenResX, player.scaleY / screenResY, 1.0));
    unsigned int transformLoc = glGetUniformLocation(playerProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    
    glBindVertexArray(playerVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawTube(GameObject& tube)
{
    glUseProgram(tubeProgram);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(tube.positionX, tube.positionY, 0.0));
    trans = glm::rotate(trans, glm::radians(tube.rotation), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(tube.scaleX, tube.scaleY, 1.0));
    unsigned int transformLoc = glGetUniformLocation(tubeProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tubeTexture);
    glBindVertexArray(tubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawBackground()
{
    glUseProgram(backgroundProgram);
    glBindVertexArray(backgroundVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawGameObjects(std::vector<GameObject>& objects, const std::chrono::microseconds& deltaTime)
{
    for (auto& object : objects)
    {
        if (object.id == GameObjectType::player)
            drawPlayer(object, deltaTime);
        else if (object.id == GameObjectType::tube)
            drawTube(object);
    }
}

void Renderer::drawTexture(const char* file)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    float vertices[] = {
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int program = shaderStuff("..\\Shaders\\texture.vert", "..\\Shaders\\texture.frag");
    

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
}

void Renderer::drawTriangle()
{
    float vertices[] = {
        0.0f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int program = shaderStuff("..\\Shaders\\simple.vert", "..\\Shaders\\simple.frag");

    glUseProgram(program);

    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //glBindVertexArray(0);
}

void Renderer::drawWidget()
{
    /*ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool show_another_window = true;
    //ImGui::ShowDemoWindow(&show_another_window);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(-5, 0));
    ImGui::Begin("Another Window", &show_another_window, windowFlags);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    //ImGui::Text("Hello from another window!");

    //ImVec2 size((float)width / 4, (float)height / 4);
    //ImVec2 uv0(0.0f, 0.0f);
    //ImVec2 uv1((float)1, (float)1);
    //ImGui::SetCursorPos(ImVec2(0, 0));
    //if (ImGui::ImageButton((ImTextureID)texture, size, uv0, uv1));
    //ImGui::End();

    ImGui::Render();
    int display_w, display_h;
    //glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
}

void Renderer::genShit()
{
    program = shaderStuff("..\\Shaders\\texture.vert", "..\\Shaders\\texture.frag");

    float vertices[] = {
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,  // top left 
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("..\\Resources\\Sprites\\background-day.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "texture1"), 0);
}

void Renderer::drawShit()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUseProgram(program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned int Renderer::shaderStuff(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    return compileShader(vShaderCode, fShaderCode);
}

unsigned int Renderer::compileShader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed, YOU'RE FUCKED BOIIIEEE\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed, YOU'RE FUCKED BOIIIEEE\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed, YOU'RE FUCKED BOIIIEEE\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}