#pragma once


class Ball : public GameObject
{
public:

	virtual ~Ball()	{		SDL_Log("Player::~Player");	}

	virtual void Init(Vector2D position)
	{
		SDL_Log("Player::Init");
		GameObject::Init();

		this->position = position;
		id = "Ball";
	}

	virtual void Receive(Message m) 
	{

	}
};

class BallBehaviourComponent : public Component
{
	ObjectPool<Ball> * ball_pool;

public:
	virtual ~BallBehaviourComponent() {}

	virtual void Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<Ball> * ball_pool)
	{
		Component::Create(engine, go, game_objects);
		this->ball_pool = ball_pool;
	}

	virtual void Update(double dt)
	{
	}
};