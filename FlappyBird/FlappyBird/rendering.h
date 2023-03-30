#pragma once

#include <chrono>
#include <vector>
#include "gameobject.h"

class Renderer {
public:
	unsigned int screenResX;
	unsigned int screenResY;
	int playerWidth, playerHeight, tubeWidth, tubeHeight, groundWidth, groundHeight, numberWidth, numberHeight;

	Renderer(unsigned int resx, unsigned int resy);
	~Renderer();
	void drawTexture(const char* file);
	void drawTriangle();
	void drawWidget();

	void genShit();
	void drawShit();

	void initializeData();
	void drawPlayer(GameObject& player, const std::chrono::microseconds& deltaTime);
	void drawTube(GameObject& tube);
	void drawGround(GameObject& ground, const std::chrono::microseconds& deltaTime);
	void drawBackground();
	void drawScore(float posx, float posy, float scalex, float scaley, int score);
	void drawGameObjects(std::vector<GameObject>& objects, const std::chrono::microseconds& deltaTime);
private:
	void loadImage(const char* data, unsigned int& texture, int& width, int& height, bool flip);
	unsigned int shaderStuff(const char* vertexPath, const char* fragmentPath);
	unsigned int compileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
	unsigned int VAO, VBO, texture1, texture2, program;

	unsigned int playerVAO, playerVBO, playerTexture1, playerTexture2, playerTexture3, playerProgram;
	unsigned int groundVAO, groundVBO, groundTexture, groundProgram;
	unsigned int tubeVAO, tubeVBO, tubeTexture, tubeProgram;
	unsigned int backgroundVAO, backgroundVBO, backgroundTexture, backgroundProgram;
	unsigned int numberVAO, numberVBO, numberTexture0, numberTexture1, numberTexture2, numberTexture3, numberTexture4, numberTexture5, numberTexture6,
		numberTexture7, numberTexture8, numberTexture9, numberProgram;

	std::chrono::microseconds groundTimePeriod = std::chrono::microseconds(3000000);
};