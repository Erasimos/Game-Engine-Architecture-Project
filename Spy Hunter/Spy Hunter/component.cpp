#include "component.h"
#include "game_object.h"
#include "avancezlib.h"
#include <algorithm>
#include <iostream>

void Component::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects)
{
	this->go = go;
	this->engine = engine;
	this->game_objects = game_objects;
}

void RenderComponent::Create(AvancezLib * engine, GameObject * go, std::set<GameObject*>* game_objects, const char * sprite_name)
{
	Component::Create(engine, go, game_objects);

	sprite = engine->createSprite(sprite_name);
}

void RenderComponent::Update(double dt)
{
	if (!go->enabled)
		return;

	if (sprite) 
		sprite->draw(int(go->position.x), int(go->position.y));
}

void RenderComponent::Destroy()
{
	if (sprite != NULL)
		sprite->destroy();
	sprite = NULL;
}


void CollideComponent::Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
}


void CollideComponent::Update(double dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); i++)
	{
		GameObject * go0 = coll_objects->pool[i];
		if (go0->enabled)
		{
			if ((go0->position.x > go->position.x - 10) &&
				(go0->position.x < go->position.x + 10) &&
				(go0->position.y > go->position.y - 10) &&
				(go0->position.y < go->position.y + 10))
			{
				go->Receive(HIT);
				go0->Receive(HIT);
			}
		}
	}
}

void CircleCollideComponent::Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, ObjectPool<GameObject> * coll_objects, double radius, UniformGrid* uniform_grid)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
	this->radius = radius;
	this->uniform_grid = uniform_grid;
}


void CircleCollideComponent::Update(double dt)
{

	//If we are using a uniform grid, instead query the grid. The grid should then return a number of potential ball objects that we then check for collisions with.
	std::set<GameObject*> balls = {};
	int cell_index = this->uniform_grid->get_cell_index(go->position);
	this->uniform_grid->get_neighbours(cell_index, &balls);

	for (auto ball = balls.begin(); ball != balls.end(); ball++)
	{
		GameObject* go0 = (*ball);

		if (go0 != go && go0->enabled)
		{
			CircleCollideComponent* otherCollider = go0->GetComponent<CircleCollideComponent*>();
			if (otherCollider != nullptr) { //if the other GameObject doesn't have a CircleColliderComponent we shouldn't go in here...

				//Write your solution here...
				double distanceBetweenCircleCenters = go->position.distance_between_vectors(go0->position);

				bool intersection = false;

				if (distanceBetweenCircleCenters <= radius + otherCollider->radius) {	
					intersection = true;	
				}
										
				if (intersection) {

					SDL_Log("Intersection");

					RigidBodyComponent * rigidBodyComponent = go->GetComponent<RigidBodyComponent*>();
					RigidBodyComponent * rigidBodyComponent0 = go0->GetComponent<RigidBodyComponent*>();

					Vector2D goTogo0 = go0->position - go->position;
					Vector2D goTogo0Normalized = goTogo0 / distanceBetweenCircleCenters; 

					double dotProduct = rigidBodyComponent->velocity.dotProduct(goTogo0Normalized);
					double dotProduct0 = rigidBodyComponent0->velocity.dotProduct(goTogo0Normalized);

					rigidBodyComponent->velocity = goTogo0Normalized * dotProduct0;
					rigidBodyComponent0->velocity = rigidBodyComponent0->velocity - goTogo0Normalized * dotProduct;
				}
			}
		}
	}

	// Update ball cell index
	int new_cell_index = uniform_grid->get_cell_index(go->position);

	if (cell_index != new_cell_index)
	{
		uniform_grid->move_ball(go, cell_index, new_cell_index);
	}
}


void RigidBodyComponent::Create(AvancezLib* engine, GameObject * go, std::set<GameObject*> * game_objects, UniformGrid* uniform_grid)
{
	Component::Create(engine, go, game_objects);
	this->uniform_grid = uniform_grid;
}

void RigidBodyComponent::Update(double dt)
{

	int cell_index = uniform_grid->get_cell_index(go->position);

	//Forward Euler method:
	go->position = go->position + velocity * dt;

	// Increase energy in the system each step,  for fun
	//velocity = velocity.operator*(Vector2D(1.0001, 1.0001));

	// Update ball cell index
	int new_cell_index = uniform_grid->get_cell_index(go->position);

	if (cell_index != new_cell_index)
	{
		uniform_grid->move_ball(go, cell_index, new_cell_index);
	}
}

void BoxCollideComponent::Create(AvancezLib* engine, GameObject* go, std::set<GameObject*>* game_objects, ObjectPool<GameObject>* coll_objects,  double width, double height)
{
	Component::Create(engine, go, game_objects);
	this->coll_objects = coll_objects;
	this->width = width;
	this->height = height;
}

void BoxCollideComponent::Update(double dt)
{
	for (auto i = 0; i < coll_objects->pool.size(); ++i)
	{
		GameObject* go0 = coll_objects->pool[i];

		if (go0 != go && go0->enabled)
		{
			CircleCollideComponent* ball_collider = go0->GetComponent<CircleCollideComponent*>();
			if (ball_collider != nullptr)
			{
				bool reflect = false;
				Vector2D n;

				if (go0->position.x < go->position.x)
				{
					reflect = true;
					n = Vector2D(1, 0);
					go0->position.x = go->position.x;
				}
				else if (go0->position.x + 32 > go->position.x + width)
				{
					reflect = true;
					n = Vector2D(-1, 0);
					go0->position.x = go->position.x + width - 32;
				}
				else if (go0->position.y < go->position.y)
				{
					reflect = true;
					n = Vector2D(0, 1);
					go0->position.y = go->position.y;
				}
				else if (go0->position.y + 32 > go->position.y + height)
				{
					reflect = true;
					n = Vector2D(0, -1);
					go0->position.y = go->position.y + height - 32;
				}

				if (reflect)
				{
					SDL_Log("Reflect");
					RigidBodyComponent* ball_body = go0->GetComponent<RigidBodyComponent*>();
					double dn = 2 * ball_body->velocity.dotProduct(n);
					ball_body->velocity = ball_body->velocity - n.operator*(dn);

				}
			}
		}
	}
}
