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
    unsigned char* fileData = stbi_load(data, &width, &height, &nrChannels, 4);
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
    playerWidth = width;
    playerHeight = height;

    // these vertices are in "local space"
    float playerVertices[] = {
        width, height, 0.0f,    1.0f, 1.0f, // top right
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, height, 0.0f,    0.0f, 1.0f,  // top left
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, -height, 0.0f,    0.0f, 0.0f, // bottom left
        -width, height, 0.0f,    0.0f, 1.0f  // top left
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
    tubeWidth = width;
    tubeHeight = height;

    // these vertices are in "local space"
    float tubeVertices[] = {
        width, height, 0.0f,    1.0f, 1.0f, // top right
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, height, 0.0f,    0.0f, 1.0f,  // top left
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, -height, 0.0f,    0.0f, 0.0f, // bottom left
        -width, height, 0.0f,    0.0f, 1.0f  // top left
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

    // ground
    loadImage("..\\Resources\\Sprites\\base.png", groundTexture, width, height, true);
    groundWidth = width;
    groundHeight = height;

    float groundVertices[] = {
        width, height, 0.0f,    1.0f, 1.0f, // top right
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, height, 0.0f,    0.0f, 1.0f,  // top left
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, -height, 0.0f,    0.0f, 0.0f, // bottom left
        -width, height, 0.0f,    0.0f, 1.0f  // top left
    };
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    groundProgram = shaderStuff("..\\Shaders\\parallax.vert", "..\\Shaders\\texture.frag");

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

    // game start screen
    loadImage("..\\Resources\\Sprites\\message.png", gameStartTexture, width, height, true);

    glGenVertexArrays(1, &gameStartVAO);
    glGenBuffers(1, &gameStartVBO);
    glBindVertexArray(gameStartVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gameStartVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // numbers
    loadImage("..\\Resources\\Sprites\\0.png", numberTexture0, width, height, true);
    loadImage("..\\Resources\\Sprites\\1.png", numberTexture1, width, height, true);
    loadImage("..\\Resources\\Sprites\\2.png", numberTexture2, width, height, true);
    loadImage("..\\Resources\\Sprites\\3.png", numberTexture3, width, height, true);
    loadImage("..\\Resources\\Sprites\\4.png", numberTexture4, width, height, true);
    loadImage("..\\Resources\\Sprites\\5.png", numberTexture5, width, height, true);
    loadImage("..\\Resources\\Sprites\\6.png", numberTexture6, width, height, true);
    loadImage("..\\Resources\\Sprites\\7.png", numberTexture7, width, height, true);
    loadImage("..\\Resources\\Sprites\\8.png", numberTexture8, width, height, true);
    loadImage("..\\Resources\\Sprites\\9.png", numberTexture9, width, height, true);
    numberWidth = width;
    numberHeight = height;

    float numberVertices[] = {
        width, height, 0.0f,    1.0f, 1.0f, // top right
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, height, 0.0f,    0.0f, 1.0f,  // top left
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, -height, 0.0f,    0.0f, 0.0f, // bottom left
        -width, height, 0.0f,    0.0f, 1.0f  // top left
    };

    glGenVertexArrays(1, &numberVAO);
    glGenBuffers(1, &numberVBO);
    glBindVertexArray(numberVAO);
    glBindBuffer(GL_ARRAY_BUFFER, numberVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(numberVertices), numberVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numberProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");

    loadImage("..\\Resources\\Sprites\\gameover.png", gameOverTexture, width, height, true);
    gameOverWidth = width;
    gameOverHeight = height;

    float gameOverVertices[] = {
        width, height, 0.0f,    1.0f, 1.0f, // top right
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, height, 0.0f,    0.0f, 1.0f,  // top left
        width, -height, 0.0f,    1.0f, 0.0f, // bottom right
        -width, -height, 0.0f,    0.0f, 0.0f, // bottom left
        -width, height, 0.0f,    0.0f, 1.0f  // top left
    };

    glGenVertexArrays(1, &gameOverVAO);
    glGenBuffers(1, &gameOverVBO);
    glBindVertexArray(gameOverVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gameOverVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gameOverVertices), gameOverVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    gameOverProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");

    initializeAtlasData();
}

void Renderer::initializeAtlasData()
{
    int atlasWidth, atlasHeight;
    loadImage("..\\Resources\\Sprites\\atlas.png", atlasTexture, atlasWidth, atlasHeight, false);

    // game over menu
    gameOverPanelWidth = 238;
    gameOverPanelHeight = 126;

    // 0.0 0.50390625 0.23242188 0.123046875
    float gameOverTopLeftX = 0.0f;
    float gameOverTopLeftY = 0.50390625f;
    float gameOverSpanX = 0.23242188f;
    float gameOverSpanY = 0.123046875f;

    float gameOverPanelVertices[] = {
        gameOverPanelWidth, gameOverPanelHeight, 0.0f,    gameOverTopLeftX + gameOverSpanX, gameOverTopLeftY, // top right
        gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    gameOverTopLeftX + gameOverSpanX, gameOverTopLeftY + gameOverSpanY, // bottom right
        -gameOverPanelWidth, gameOverPanelHeight, 0.0f,    gameOverTopLeftX, gameOverTopLeftY,  // top left
        gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    gameOverTopLeftX + gameOverSpanX, gameOverTopLeftY + gameOverSpanY, // bottom right
        -gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    gameOverTopLeftX, gameOverTopLeftY + gameOverSpanY, // bottom left
        -gameOverPanelWidth, gameOverPanelHeight, 0.0f,    gameOverTopLeftX, gameOverTopLeftY  // top left
    };

    //float gameOverPanelVertices[] = {
    //    gameOverPanelWidth, gameOverPanelHeight, 0.0f,    1.0f, 0.0f, // top right
    //    gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    1.0f, 1.0f, // bottom right
    //    -gameOverPanelWidth, gameOverPanelHeight, 0.0f,    0.0f, 0.0f,  // top left
    //    gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    1.0f, 1.0f, // bottom right
    //    -gameOverPanelWidth, -gameOverPanelHeight, 0.0f,    0.0f, 1.0f, // bottom left
    //    -gameOverPanelWidth, gameOverPanelHeight, 0.0f,    0.0f, 0.0f  // top left
    //};

    glGenVertexArrays(1, &gameOverPanelVAO);
    glGenBuffers(1, &gameOverPanelVBO);
    glBindVertexArray(gameOverPanelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gameOverPanelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gameOverPanelVertices), gameOverPanelVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    gameOverPanelProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");

    medalWidth = 44;
    medalHeight = 44;
    // bronze medal
    float bronzeMedalTopLeftX = 0.21875;
    float bronzeMedalTopLeftY = 0.9316406;
    float bronzeMedalSpanX = 0.04296875;
    float bronzeMedalSpanY = 0.04296875;
    float bronzeMedalVertices[] = {
        medalWidth, medalHeight, 0.0f,    bronzeMedalTopLeftX + bronzeMedalSpanX, bronzeMedalTopLeftY, // top right
        medalWidth, -medalHeight, 0.0f,    bronzeMedalTopLeftX + bronzeMedalSpanX, bronzeMedalTopLeftY + bronzeMedalSpanY, // bottom right
        -medalWidth, medalHeight, 0.0f,    bronzeMedalTopLeftX, bronzeMedalTopLeftY,  // top left
        medalWidth, -medalHeight, 0.0f,    bronzeMedalTopLeftX + bronzeMedalSpanX, bronzeMedalTopLeftY + bronzeMedalSpanY, // bottom right
        -medalWidth, -medalHeight, 0.0f,    bronzeMedalTopLeftX, bronzeMedalTopLeftY + bronzeMedalSpanY, // bottom left
        -medalWidth, medalHeight, 0.0f,    bronzeMedalTopLeftX, bronzeMedalTopLeftY  // top left
    };

    glGenVertexArrays(1, &bronzeMedalVAO);
    glGenBuffers(1, &bronzeMedalVBO);
    glBindVertexArray(bronzeMedalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bronzeMedalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bronzeMedalVertices), bronzeMedalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    medalProgram = shaderStuff("..\\Shaders\\texture-transform.vert", "..\\Shaders\\texture.frag");
    
    // silver medal
    float silverMedalTopLeftX = 0.23632812;
    float silverMedalTopLeftY = 0.50390625;
    float silverMedalSpanX = 0.04296875;
    float silverMedalSpanY = 0.04296875;
    float silverMedalVertices[] = {
        medalWidth, medalHeight, 0.0f,    silverMedalTopLeftX + silverMedalSpanX, silverMedalTopLeftY, // top right
        medalWidth, -medalHeight, 0.0f,    silverMedalTopLeftX + silverMedalSpanX, silverMedalTopLeftY + silverMedalSpanY, // bottom right
        -medalWidth, medalHeight, 0.0f,    silverMedalTopLeftX, silverMedalTopLeftY,  // top left
        medalWidth, -medalHeight, 0.0f,    silverMedalTopLeftX + silverMedalSpanX, silverMedalTopLeftY + silverMedalSpanY, // bottom right
        -medalWidth, -medalHeight, 0.0f,    silverMedalTopLeftX, silverMedalTopLeftY + silverMedalSpanY, // bottom left
        -medalWidth, medalHeight, 0.0f,    silverMedalTopLeftX, silverMedalTopLeftY  // top left
    };

    glGenVertexArrays(1, &silverMedalVAO);
    glGenBuffers(1, &silverMedalVBO);
    glBindVertexArray(silverMedalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, silverMedalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(silverMedalVertices), silverMedalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // gold
    float goldMedalTopLeftX = 0.23632812;
    float goldMedalTopLeftY = 0.55078125;
    float goldMedalSpanX = 0.04296875;
    float goldMedalSpanY = 0.04296875;
    float goldMedalVertices[] = {
        medalWidth, medalHeight, 0.0f,    goldMedalTopLeftX + goldMedalSpanX, goldMedalTopLeftY, // top right
        medalWidth, -medalHeight, 0.0f,    goldMedalTopLeftX + goldMedalSpanX, goldMedalTopLeftY + goldMedalSpanY, // bottom right
        -medalWidth, medalHeight, 0.0f,    goldMedalTopLeftX, goldMedalTopLeftY,  // top left
        medalWidth, -medalHeight, 0.0f,    goldMedalTopLeftX + goldMedalSpanX, goldMedalTopLeftY + goldMedalSpanY, // bottom right
        -medalWidth, -medalHeight, 0.0f,    goldMedalTopLeftX, goldMedalTopLeftY + goldMedalSpanY, // bottom left
        -medalWidth, medalHeight, 0.0f,    goldMedalTopLeftX, goldMedalTopLeftY  // top left
    };

    glGenVertexArrays(1, &goldMedalVAO);
    glGenBuffers(1, &goldMedalVBO);
    glBindVertexArray(goldMedalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, goldMedalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(goldMedalVertices), goldMedalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
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
    trans = glm::translate(trans, glm::vec3(player.positionX / screenResX * 2.0f - 1.0f, player.positionY / screenResY * 2.0f - 1.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(player.scaleX / screenResX, player.scaleY / screenResY, 1.0));
    trans = glm::rotate(trans, glm::radians(player.rotation), glm::vec3(0.0, 0.0, 1.0));
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
    
    trans = glm::translate(trans, glm::vec3(tube.positionX / screenResX * 2.0f - 1.0f, tube.positionY / screenResY * 2.0f - 1.0f, 0.0));
    trans = glm::scale(trans, glm::vec3(tube.scaleX / screenResX, tube.scaleY / screenResY, 1.0));
    trans = glm::rotate(trans, glm::radians(tube.rotation), glm::vec3(0.0, 0.0, 1.0));
    
    unsigned int transformLoc = glGetUniformLocation(tubeProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tubeTexture);
    glBindVertexArray(tubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawGround(GameObject& ground, const std::chrono::microseconds& deltaTime)
{
    static float delta = 0.0f;
    glUseProgram(groundProgram);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(ground.positionX / screenResX * 2.0f - 1.0f, ground.positionY / screenResY * 2.0f - 1.0f, 0.0));
    trans = glm::rotate(trans, glm::radians(ground.rotation), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(ground.scaleX / screenResX, ground.scaleY / screenResY, 1.0));
    unsigned int transformLoc = glGetUniformLocation(groundProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    unsigned int timeLoc = glGetUniformLocation(groundProgram, "time");
    delta += (float)deltaTime.count() / groundTimePeriod.count();
    if (delta >= 1.0f)
        delta = 0.0f;
    glUniform1f(timeLoc, delta);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glBindVertexArray(groundVAO);
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

void Renderer::drawGameStart()
{
    glUseProgram(backgroundProgram);
    glBindVertexArray(gameStartVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gameStartTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawScore(float posx, float posy, float scalex, float scaley, int score)
{
    unsigned int texture;
    switch (score)
    {
    case 0:
        texture = numberTexture0;
        break;
    case 1:
        texture = numberTexture1;
        break;
    case 2:
        texture = numberTexture2;
        break;
    case 3:
        texture = numberTexture3;
        break;
    case 4:
        texture = numberTexture4;
        break;
    case 5:
        texture = numberTexture5;
        break;
    case 6:
        texture = numberTexture6;
        break;
    case 7:
        texture = numberTexture7;
        break;
    case 8:
        texture = numberTexture8;
        break;
    case 9:
        texture = numberTexture9;
        break;
    default:
        texture = numberTexture0;
        break;
    }

    glUseProgram(numberProgram);
    glm::mat4 trans = glm::mat4(1.0f);

    trans = glm::translate(trans, glm::vec3(posx / screenResX * 2.0f - 1.0f, posy / screenResY * 2.0f - 1.0f, 0.0));
    trans = glm::scale(trans, glm::vec3(scalex / screenResX, scaley / screenResY, 1.0));
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));

    unsigned int transformLoc = glGetUniformLocation(numberProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(numberVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawUIElement(UIElementType type, float posx, float posy, float scalex, float scaley)
{
    unsigned int program, texture, vao;

    switch (type)
    {
    case gameOverText:
        program = gameOverProgram;
        texture = gameOverTexture;
        vao = gameOverVAO;
        break;
    case gameOverPanel:
        program = gameOverPanelProgram;
        texture = atlasTexture;
        vao = gameOverPanelVAO;
        break;
    case bronzeMedal:
        program = medalProgram;
        texture = atlasTexture;
        vao = bronzeMedalVAO;
        break;
    case silverMedal:
        program = medalProgram;
        texture = atlasTexture;
        vao = silverMedalVAO;
        break;
    case goldMedal:
        program = medalProgram;
        texture = atlasTexture;
        vao = goldMedalVAO;
        break;
    }

    glUseProgram(program);
    glm::mat4 trans = glm::mat4(1.0f);

    trans = glm::translate(trans, glm::vec3(posx / screenResX * 2.0f - 1.0f, posy / screenResY * 2.0f - 1.0f, 0.0));
    trans = glm::scale(trans, glm::vec3(scalex / screenResX, scaley / screenResY, 1.0));
    trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));

    unsigned int transformLoc = glGetUniformLocation(program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawGameObjects(std::vector<GameObject>& objects, const std::chrono::microseconds& deltaTime)
{
    GameObject* player = nullptr;
    GameObject* ground = nullptr;

    for (auto& object : objects)
    {
        if (object.id == GameObjectType::player)
        {
            player = &object;
        }
        else if (object.id == GameObjectType::tube)
        {
            drawTube(object);
        }
        else if (object.id == GameObjectType::ground)
        {
            ground = &object;
        }
    }

    // want to draw these objects last
    if (ground)
        drawGround(*ground, deltaTime);
    if (player)
        drawPlayer(*player, deltaTime);
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