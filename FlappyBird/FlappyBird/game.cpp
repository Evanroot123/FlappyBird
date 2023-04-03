#include "game.h"
#include <iostream>
#include "rendering.h"
#include <random>

Game::Game(unsigned int worx, unsigned int wory, Renderer& render, irrklang::ISoundEngine* engine) :
	worldSpaceX(worx), worldSpaceY(wory), renderer(render), speed(500.0f), playerVelocity(0.0f), playerAcceleration(-10.0), soundEngine(engine)
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
	float startingXPosition = worldSpaceX + renderer.tubeWidth / 2.0f;

	spawnTubes();

	scorePosition = startingXPosition;
}

void Game::update(std::chrono::microseconds deltaTime)
{
	if (gameEnd)
	{
		return;
	}

	float delta = deltaTime.count() / 1000000.0f;

	// for manual movement
	//gameObjects[0].positionX += playerDirX * speed * delta;
	//gameObjects[0].positionY += playerDirY * speed * delta;
	
	// gravity + jumping
	playerVelocity += playerAcceleration * delta;
	gameObjects[0].positionY += playerVelocity;

	playerDirX = 0;
	playerDirY = 0;

	// set player rotation based off their velocity
	std::cout << playerVelocity << std::endl;
	float velocity = playerVelocity;
	//if (playerVelocity / 2 < -90.0f)
	//	velocity = -90.0f;

	gameObjects[0].rotation = 80.0f * sin(12 * velocity * 3.14159265358979323846 / 180);
	if (gameObjects[0].rotation > maxPlayerRotation)
		gameObjects[0].rotation = maxPlayerRotation;
	else if (gameObjects[0].rotation < minPlayerRotation)
		gameObjects[0].rotation = minPlayerRotation;

	if (isPlayerTouchingGround())
	{
		std::cout << " hit the ground" << std::endl;
		gameOver();
	}

	// update tubes position
	for (auto& object : gameObjects)
	{
		if (object.id == GameObjectType::tube)
		{
			object.positionX -= tubeSpeed * delta;
			if (isPlayerOverlapping(object))
			{
				std::cout << "tube" << std::endl;
				gameOver();
			}
		}
	}

	// need to check if last tube x pos is greater than horizontal tube distance away from edge of screen
	GameObject& tube = gameObjects.back();
	float tubeRightEdge = tube.positionX + tube.textureSizeX * tube.scaleX / 2.0f;
	if (worldSpaceX - tubeRightEdge >= horizontalDistanceBetweenTubes)
		spawnTubes();

	// check if tubes need to be despawned
	// first two objects are player and ground
	for (auto i = gameObjects.end() - 1; i > gameObjects.begin() + 1; i--)
	{
		if (i->id == GameObjectType::tube)
		{
			float tubeRightEdge = i->positionX + i->textureSizeX * i->scaleX / 2.0f;
			if (tubeRightEdge < 0)
			{
				i = gameObjects.erase(i);
			}
		}
	}

	scorePosition -= tubeSpeed * delta;
	if (scorePosition - gameObjects[0].positionX <= 0)
	{
		soundEngine->play2D("..\\Resources\\Audio\\point.mp3");
		playerScore++;
		GameObject& tube = gameObjects[2];
		scorePosition += tube.textureSizeX * tube.scaleX + horizontalDistanceBetweenTubes;
		std::cout << "score: " << playerScore << std::endl;
	}
}

void Game::spawnTubes()
{
	float startingXPosition = worldSpaceX + renderer.tubeWidth / 2.0f;
	float ground = gameObjects[1].textureSizeY * gameObjects[1].scaleY;

	GameObject tube1{ startingXPosition, 0, 1.0, 1.0, 0, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight };
	GameObject tube2{ startingXPosition, 0, 1.0, 1.0, 180, GameObjectType::tube, renderer.tubeWidth, renderer.tubeHeight };

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

void Game::despawnTubes()
{

}

void Game::gameOver()
{
	std::cout << "GAME OVER" << std::endl;
	gameEnd = true;
	// stop updating the scene
	// show game over + final score
}

void Game::playerJump()
{
	playerVelocity = 3.0f;
	soundEngine->play2D("..\\Resources\\Audio\\wing.mp3");
}

void Game::playerMove(int x, int y)
{
	// adding to account for multiple keys being pressed per step, for wasd
	playerDirX += x;
	playerDirY += y;
}

bool Game::isPlayerTouchingGround()
{
	// player Y position below certain value (hitting the floor)
	float ground = gameObjects[1].textureSizeY * gameObjects[1].scaleY;
	float halfPlayerSize = gameObjects[0].textureSizeY * gameObjects[0].scaleY / 2.0f;
	float playerBottomEdge = gameObjects[0].positionY - halfPlayerSize;

	if (playerBottomEdge <= ground)
		return true;

	return false;
}

// collision detection for tubes and floor
bool Game::isPlayerOverlapping(GameObject& object)
{
	if (object.id == GameObjectType::tube)
	{
		float halfPlayerSizeX = gameObjects[0].textureSizeX * gameObjects[0].scaleX / 2.0f;
		float halfTubeSizeX = object.textureSizeX * object.scaleX / 2.0f;
		float posDiffX = abs(gameObjects[0].positionX - object.positionX);

		if (posDiffX <= halfPlayerSizeX + halfTubeSizeX)
		{
			// special case 1 - player Y position above certain value and player x position overlapping with tube (too high)
			float halfPlayerSizeY = gameObjects[0].textureSizeY * gameObjects[0].scaleY / 2.0f;
			float playerTopEdge = gameObjects[0].positionY + halfPlayerSizeY;
			float maxHeight = worldSpaceY - tubeMaxYExtent;

			if (playerTopEdge >= maxHeight)
				return true;
		}
		else
		{
			return false;
		}

		float halfPlayerSizeY = gameObjects[0].textureSizeY * gameObjects[0].scaleY / 2.0f;
		float halfTubeSizeY = object.textureSizeY * object.scaleY / 2.0f;
		float posDiffY = abs(gameObjects[0].positionY - object.positionY);

		if (posDiffY <= halfPlayerSizeY + halfTubeSizeY)
		{
			return true;
		}

		return false;
	}

	return false;
}