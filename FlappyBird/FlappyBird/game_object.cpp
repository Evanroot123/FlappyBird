#include "game_object.h"
#include "stb_image.h"
#include <iostream>

GameObject::GameObject(char* file)
{
	int width, height, nrChannels;
	textureData = stbi_load(file, &width, &height, &nrChannels, 0);

	std::cout << nrChannels << std::endl;
}

GameObject::~GameObject()
{
	stbi_image_free(textureData);
}