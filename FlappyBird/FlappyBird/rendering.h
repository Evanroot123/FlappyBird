#pragma once

#include <chrono>

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
	void drawPlayer(std::chrono::microseconds deltaTime);
private:
	void loadImage(const char* data, unsigned int& texture, bool flip);
	unsigned int shaderStuff(const char* vertexPath, const char* fragmentPath);
	unsigned int compileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
	unsigned int VAO, VBO, texture1, texture2, program;

	unsigned int playerVAO, playerVBO, playerTexture1, playerTexture2, playerTexture3, playerProgram;
	unsigned int groundVAO, groundVBO, groundTexture1, groundTexture2, groundTexture3, groundProgram;
	unsigned int tube1VAO, tube1VBO, tube1Texture1, tube1Texture2, tube1Texture3, tube1Program;
	unsigned int tube2VAO, tube2VBO, tube2Texture1, tube2Texture2, tube2Texture3, tube2Program;
	unsigned int tube3VAO, tube3VBO, tube3Texture1, tube3Texture2, tube3Texture3, tube3Program;
	unsigned int tube4VAO, tube4VBO, tube4Texture1, tube4Texture2, tube4Texture3, tube4Program;
};