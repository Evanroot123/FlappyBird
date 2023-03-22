#pragma once
enum GameObjectType { player, tube, ground };

class GameObject {
public:
	float positionX;
	float positionY;
	float scaleX;
	float scaleY;
	float rotation;
	GameObjectType id;

	GameObject(float posx, float posy, float scax, float scay, float rot, GameObjectType _id);
};