#include "game.h"
#include <iostream>

Game::Game(unsigned int worx, unsigned int wory) : worldSpaceX(worx), worldSpaceY(wory), speed(500.0f), playerVelocity(0.0f), playerAcceleration(-13.0)
{
	// first object is player
	gameObjects.push_back(GameObject{ 100.0, 300.0, 1.3, 1.3, 0, GameObjectType::player });

	// tubes
	//gameObjects.push_back(GameObject{ 0.5, 0.5, 0.2, 0.8, 180, GameObjectType::tube });
}

void Game::update(std::chrono::microseconds deltaTime)
{
	float delta = deltaTime.count() / 1000000.0f;

	// for manual movement
	//gameObjects[0].positionX += playerDirX * speed * delta;
	//gameObjects[0].positionY += playerDirY * speed * delta;

	playerVelocity += playerAcceleration * delta;
	gameObjects[0].positionY += playerVelocity;

	playerDirX = 0;
	playerDirY = 0;
}

void Game::playerJump()
{
	playerVelocity = 4.1f;
}

void Game::playerMove(int x, int y)
{
	// adding to account for multiple keys being pressed per step, for wasd
	playerDirX += x;
	playerDirY += y;
}