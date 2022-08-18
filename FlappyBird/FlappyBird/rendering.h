#pragma once

class Renderer {
public:
	Renderer();
	~Renderer();
	void drawTexture(const char* file);
	void drawTriangle();
	void drawWidget();
private:
	void loadImages();
	unsigned int shaderStuff(const char* vertexPath, const char* fragmentPath);
	unsigned int compileShader(const char* vertexShaderSource, const char* fragmentShaderSource);
};