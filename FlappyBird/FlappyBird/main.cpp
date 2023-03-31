#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <fstream>

#include "rendering.h"
#include "game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Game& game);
void drawScore(Renderer& renderer, int score, int posx, int posy);

const unsigned int SCREEN_WIDTH = 400;
const unsigned int SCREEN_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GuiTesting", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // imgui
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    std::fstream scoreFile;
    scoreFile.open("..\\score.txt");
    std::string scoreLine;
    std::getline(scoreFile, scoreLine);
    int highScore = 0;
    if (scoreLine.length() > 0)
        highScore = std::stoi(scoreLine);
    scoreFile.close();

    std::cout << "Current high score: " << highScore << std::endl;

    // basing the world coordinates off the actual pixels of the screen, this is super convenient for testing,
    // but could have unintended side effects later
    Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT);
    renderer.initializeData();
    Game game(SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
    game.start();

    int frames = 0;
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::chrono::microseconds timeCollector = std::chrono::microseconds(0);

    float endGameScoreDelta = 0.0f;
    bool onGameEnd = true;

    while (!glfwWindowShouldClose(window))
    {
        frames++;
        end = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        start = std::chrono::steady_clock::now();
        timeCollector += duration;

        if (timeCollector >= std::chrono::microseconds(1000000))
        {
            std::cout << "average fps: " << frames << std::endl;
            timeCollector = std::chrono::microseconds(0);
            frames = 0;
        }

        processInput(window, game);

        game.update(duration);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.drawBackground();

        // want the parallax scrolling on the ground to stop when the game ends
        if (game.gameEnd)
        {
            if (onGameEnd)
            {
                onGameEnd = false;
                if (game.playerScore > highScore)
                {
                    highScore = game.playerScore;
                    scoreFile.open("..\\score.txt");
                    if (scoreFile.is_open())
                    {
                        scoreFile << game.playerScore;
                        scoreFile.close();
                    }
                }
            }

            renderer.drawGameObjects(game.gameObjects, std::chrono::microseconds(0));
            renderer.drawUIElement(UIElementType::gameOverText, 200.0f, 450.0f, 1.0f, 1.0f);
            renderer.drawUIElement(UIElementType::gameOverPanel, 200.0f, 450.0f - renderer.gameOverHeight / 2.0f - renderer.gameOverPanelHeight * 1.5f / 2.0f - 10.0f, 1.5f, 1.5f);
            
            if (game.playerScore == highScore)
                renderer.drawUIElement(UIElementType::goldMedal, 103.0f, 319.0f, 1.5f, 1.5f);
            else if (highScore - game.playerScore < 10)
                renderer.drawUIElement(UIElementType::silverMedal, 103.0f, 319.0f, 1.5f, 1.5f);
            else
                renderer.drawUIElement(UIElementType::bronzeMedal, 103.0f, 319.0f, 1.5f, 1.5f);

            // need to show score on menu, but gradually count up to the score over the course of ~ 1 second
            if (endGameScoreDelta < 1.0f)
                endGameScoreDelta += duration.count() / 1000000.0f;
            else
                endGameScoreDelta = 1.0f;

            drawScore(renderer, game.playerScore * endGameScoreDelta, 315.0f, 350.0f);
            drawScore(renderer, highScore, 315.0f, 280.0f);
        }
        else
        {
            renderer.drawGameObjects(game.gameObjects, duration);
            drawScore(renderer, game.playerScore, SCREEN_WIDTH / 2.0f, 550.0f);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, Game& game)
{
    static bool mousePressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        game.playerMove(0, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        game.playerMove(-1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        game.playerMove(0, -1);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        game.playerMove(1, 0);
    }
    if (mousePressed)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
    else
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            game.playerJump();
            mousePressed = true;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void drawScore(Renderer& renderer, int score, int posx, int posy)
{
    int digits[10] = { 0 };
    int length = 0;
    int result = score;
    int spaceBetweenDigits = 3;

    if (score == 0)
    {
        renderer.drawScore(posx - renderer.numberWidth / 2.0f, posy, 1.0f, 1.0f, 0);
        return;
    }

    while (result > 0)
    {
        digits[length] = result % 10;
        result /= 10;
        length++;
    }

    int startPos = (posx) - ((renderer.numberWidth * length + spaceBetweenDigits * (length - 1)) / 2.0f);

    while (length > 0)
    {
        renderer.drawScore(startPos, posy, 1.0f, 1.0f, digits[length - 1]);
        startPos += renderer.numberWidth + spaceBetweenDigits;
        length--;
    }
}