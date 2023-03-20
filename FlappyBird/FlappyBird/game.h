#pragma once
#include <string>

struct Settings {
	int screenHeight;
	int screenWidth;
	std::string title;
};

class Game {

public:
	Game();
	Settings settings;
	int playerX;
	int playerY;
	int playerRotation;
};