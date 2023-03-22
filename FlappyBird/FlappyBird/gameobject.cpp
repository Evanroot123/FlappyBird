#include "gameobject.h"

GameObject::GameObject(float posx, float posy, float scax, float scay, float rot, GameObjectType _id) :
	positionX(posx), positionY(posy), scaleX(scax), scaleY(scay), rotation(rot), id(_id)
{}