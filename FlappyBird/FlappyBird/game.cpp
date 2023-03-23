#include "game.h"

Game::Game(unsigned int worx, unsigned int wory) : worldSpaceX(worx), worldSpaceY(wory), speed(5.0f)
{
	// first object is player
	gameObjects.push_back(GameObject{ 1.0, 1.0, 2.5, 2.5, 0, GameObjectType::player });

	// tubes
	//gameObjects.push_back(GameObject{ 0.5, 0.5, 0.2, 0.8, 180, GameObjectType::tube });
}

void Game::update(std::chrono::microseconds deltaTime)
{
	// update player position and speed

}

void Game::playerJump()
{

}

void Game::playerMove(int x, int y)
{
	gameObjects[0].positionX += x * speed;
	gameObjects[0].positionY += y * speed;
}