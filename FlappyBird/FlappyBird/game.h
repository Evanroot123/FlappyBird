#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "gameobject.h"

class Game {

public:
	std::vector<GameObject> gameObjects;
	float playerVelocity;
	float playerAcceleration;
	int playerDirX;
	int playerDirY;
	unsigned int worldSpaceX;
	unsigned int worldSpaceY;
	float speed;

	Game(unsigned int worx, unsigned int wory);
	void update(std::chrono::microseconds deltaTime);
	void playerJump();
	void playerMove(int x, int y);
};