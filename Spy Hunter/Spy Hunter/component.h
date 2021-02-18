#pragma once

#include <set>
#include "object_pool.h"
#include "game_object.h"
#include "uniform_grid.h"

class GameObject;
class AvancezLib;
class Sprite;
class UniformGrid;


class Component
{
protected:
	AvancezLib * engine;	// used to access the engine
	GameObject * go;		// the game object this component is part of
	std::set<GameObject*> * game_objects;	// the global container of game objects

public:
	virtual ~Component() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects);

	virtual void Init() {}
	virtual void Update(double dt) = 0;
	virtual void Receive(int message) {}
	virtual void Destroy() {}
};


class RenderComponent : public Component
{
	Sprite* sprite;

public:

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, const char * sprite_name);
	virtual void Update(double dt);
	virtual void Destroy();

	Sprite * GetSprite() { return sprite; }
};


class CollideComponent : public Component
{
protected:
	ObjectPool<GameObject> * coll_objects; // collision will be tested with these objects
	UniformGrid* uniform_grid; // collision will be tested against neighbouring cells in this grid

public:
	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects);
	virtual void Update(double dt);
};

class CircleCollideComponent : public CollideComponent
{
	double radius;
	UniformGrid* uniform_grid;

public:
	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects, double radius, UniformGrid* uniform_grid);
	virtual void Update(double dt);
};


class RigidBodyComponent : public Component
{
public:
	Vector2D velocity, acceleration;
	UniformGrid* uniform_grid;

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, UniformGrid* uniform_grid);
	virtual void Update(double dt);
};

class BoxCollideComponent : public CollideComponent
{
public:
	
	double width;
	double height;

	virtual void Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects, double width, double height);
	virtual void Update(double dt);
};
