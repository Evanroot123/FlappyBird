#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "gameobject.h"
#include "rendering.h"

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

	// tube stuff
	int horizontalDistanceBetweenTubes = 160;
	int verticalDistanceBetweenTubes = 130;
	int tubeSpawnPoint;
	int tubeDespawnPoint;
	int tubeMinYExtent = 40;
	int tubeMaxYExtent = 40;
	float tubeSpeed = 160.0f;

	float scorePosition;
	int playerScore = 0;

	bool gameEnd = false;

	Renderer& renderer;

	Game(unsigned int worx, unsigned int wory, Renderer& render);
	void start();
	void update(std::chrono::microseconds deltaTime);
	void spawnTubes();
	void despawnTubes();
	void gameOver();
	void playerJump();
	void playerMove(int x, int y);
	bool isPlayerTouchingGround();
	bool isPlayerOverlapping(GameObject& object);
};