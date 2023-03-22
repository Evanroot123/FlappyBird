#pragma once

#include <chrono>
#include <vector>
#include "gameobject.h"

class Renderer {
public:
	Renderer();
	~Renderer();
	void drawTexture(const char* file);
	void drawTriangle();
	void drawWidget();

	void genShit();
	void drawShit();

	void initializeData();
	void drawPlayer(GameObject& player, const std::chrono::microseconds& deltaTime);
	void drawTube(GameObject& tube);
	void drawGameObjects(std::vector<GameObject>& objects, const std::chrono::microseconds& deltaTime);
private:
	void loadImage(const char* data, unsigned int& texture, bool flip);
	unsigned int shaderStuff(const char* vertexPath, const char* fragmentPath);
	unsigned int compileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
	unsigned int VAO, VBO, texture1, texture2, program;

	unsigned int playerVAO, playerVBO, playerTexture1, playerTexture2, playerTexture3, playerProgram;
	unsigned int groundVAO, groundVBO, groundTexture, groundProgram;
	unsigned int tubeVAO, tubeVBO, tubeTexture, tubeProgram;
};