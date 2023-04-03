#pragma once

#include <chrono>
#include <vector>
#include "gameobject.h"

enum UIElementType { gameOverText, gameOverPanel, bronzeMedal, silverMedal, goldMedal};

class Renderer {
public:
	unsigned int screenResX;
	unsigned int screenResY;
	int playerWidth, playerHeight, tubeWidth, tubeHeight, groundWidth, groundHeight, numberWidth, numberHeight, gameOverWidth, gameOverHeight,
		gameOverPanelWidth, gameOverPanelHeight, medalWidth, medalHeight;

	Renderer(unsigned int resx, unsigned int resy);
	~Renderer();
	void drawTexture(const char* file);
	void drawTriangle();
	void drawWidget();

	void genShit();
	void drawShit();

	void initializeData();
	void initializeAtlasData();
	void drawPlayer(GameObject& player, const std::chrono::microseconds& deltaTime);
	void drawTube(GameObject& tube);
	void drawGround(GameObject& ground, const std::chrono::microseconds& deltaTime);
	void drawBackground();
	void drawGameStart();
	void drawScore(float posx, float posy, float scalex, float scaley, int score);
	void drawUIElement(UIElementType type, float posx, float posy, float scalex, float scaley);
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
	unsigned int gameStartVAO, gameStartVBO, gameStartTexture;
	unsigned int numberVAO, numberVBO, numberTexture0, numberTexture1, numberTexture2, numberTexture3, numberTexture4, numberTexture5, numberTexture6,
		numberTexture7, numberTexture8, numberTexture9, numberProgram;
	unsigned int gameOverVAO, gameOverVBO, gameOverTexture, gameOverProgram;

	// atlas
	unsigned int atlasTexture;
	unsigned int gameOverPanelVAO, gameOverPanelVBO, gameOverPanelProgram;
	unsigned int bronzeMedalVAO, bronzeMedalVBO, medalProgram;
	unsigned int silverMedalVAO, silverMedalVBO;
	unsigned int goldMedalVAO, goldMedalVBO;

	std::chrono::microseconds groundTimePeriod = std::chrono::microseconds(3000000);
};