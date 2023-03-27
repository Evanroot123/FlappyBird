#include "gameobject.h"

GameObject::GameObject(float posx, float posy, float scax, float scay, float rot, GameObjectType _id, int textx, int texty) :
	positionX(posx), positionY(posy), scaleX(scax), scaleY(scay), rotation(rot), id(_id), textureSizeX(textx), textureSizeY(texty)
{}