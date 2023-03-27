#include "game.h"
#include <iostream>
#include "rendering.h"
#include <random>

Game::Game(unsigned int worx, unsigned int wory, Renderer& render) : 
	worldSpaceX(worx), worldSpaceY(wory), renderer(render), speed(500.0f), playerVelocity(0.0f), playerAcceleration(-13.0)
{
	// first object is player
	gameObjects.push_back(GameObject{ 100.0, 300.0, 1.3, 1.3, 0, GameObjectType::player, renderer.playerWidth, renderer.playerHeight });

	// tubes
	//gameObjects.push_back(GameObject{ 200.0, 300.0, 1.0, 1.0, 0, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight });
	//gameObjects.push_back(GameObject{ 200.0, 300.0, 1.0, 1.0, 180, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight });

	// ground
	gameObjects.push_back(GameObject{ 200.0, 55.0, 1.19, 1.0, 0, GameObjectType::ground, renderer.groundWidth, renderer.groundHeight });
}

void Game::start()
{
	float ground = gameObjects[1].textureSizeY * gameObjects[1].scaleY;

	GameObject tube1{ 300.0, 0.0, 1.0, 1.0, 0, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight };
	GameObject tube2{ 300.0, 0.0, 1.0, 1.0, 180, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight };

	tube1.positionY = ground + tubeMinYExtent - (tube1.textureSizeY * tube1.scaleY / 2);
	tube2.positionY = worldSpaceY + (tube2.textureSizeY * tube2.scaleY / 2) - tubeMaxYExtent;

	// calculate a random number, and then offset each tube half verticalDistanceBetweenTubes from that point
	int range = worldSpaceY - tubeMaxYExtent - ground - tubeMinYExtent - verticalDistanceBetweenTubes;

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, range);
	int random = dist(mt);

	tube1.positionY += random;
	tube2.positionY -= range - random;

	// spawn tubes
	gameObjects.push_back(tube1);
	gameObjects.push_back(tube2);
}

void Game::update(std::chrono::microseconds deltaTime)
{
	float delta = deltaTime.count() / 1000000.0f;

	// for manual movement
	//gameObjects[0].positionX += playerDirX * speed * delta;
	//gameObjects[0].positionY += playerDirY * speed * delta;
	//
	//playerVelocity += playerAcceleration * delta;
	//gameObjects[0].positionY += playerVelocity;

	playerDirX = 0;
	playerDirY = 0;

	// update tubes position

}

void Game::spawnTubes()
{

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