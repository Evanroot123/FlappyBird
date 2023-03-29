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
	int horizontalDistanceBetweenTubes = 180;
	int verticalDistanceBetweenTubes = 150;
	int tubeSpawnPoint;
	int tubeDespawnPoint;
	int tubeMinYExtent = 40;
	int tubeMaxYExtent = 40;
	float tubeSpeed = 100.0f;

	float scorePosition;

	Renderer& renderer;

	Game(unsigned int worx, unsigned int wory, Renderer& render);
	void start();
	void update(std::chrono::microseconds deltaTime);
	void spawnTubes();
	void despawnTubes();
	void playerJump();
	void playerMove(int x, int y);
	bool isPlayerTouchingGround();
	bool isPlayerOverlapping(GameObject& object);
};