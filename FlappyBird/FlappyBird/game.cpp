#include "game.h"

Game::Game()
{
	// first object is player
	gameObjects.push_back(GameObject{ 0.0, 0.0, 0.13, 0.065, 0, GameObjectType::player });

	// tubes
	gameObjects.push_back(GameObject{ 0.5, 0.5, 0.2, 0.8, 180, GameObjectType::tube });
}

void Game::playerJump()
{

}