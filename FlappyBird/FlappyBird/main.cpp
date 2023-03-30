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

#include "rendering.h"
#include "game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Game& game);
void drawScore(Renderer& renderer, int score);

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
            renderer.drawGameObjects(game.gameObjects, std::chrono::microseconds(0));
        }
        else
        {
            renderer.drawGameObjects(game.gameObjects, duration);
        }

        drawScore(renderer, game.playerScore);
        //renderer.drawScore(SCREEN_WIDTH / 2.0f, 550.0f, 1.0f, 1.0f, 0);

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

void drawScore(Renderer& renderer, int score)
{
    int digits[10] = { 0 };
    int length = 0;
    int result = score;

    if (score == 0)
    {
        renderer.drawScore(SCREEN_WIDTH / 2.0f, 550.0f, 1.0f, 1.0f, 0);
        return;
    }

    while (result > 0)
    {
        digits[length] = result % 10;
        result /= 10;
        length++;
    }

    int startPos = (SCREEN_WIDTH / 2.0f) - (renderer.numberWidth * length / 2.0f);

    while (length > 0)
    {
        renderer.drawScore(startPos, 550.0f, 1.0f, 1.0f, digits[length - 1]);
        startPos += renderer.numberWidth;
        length--;
    }
}