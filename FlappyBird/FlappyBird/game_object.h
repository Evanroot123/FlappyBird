#pragma once
#include "glm.hpp"

class GameObject {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	unsigned char* textureData;

	GameObject(char* file);
	~GameObject();
};