#pragma once
#include <string>
#include <vector>
#include "gameobject.h"

class Game {

public:
	std::vector<GameObject> gameObjects;

	Game();
	void playerJump();
};