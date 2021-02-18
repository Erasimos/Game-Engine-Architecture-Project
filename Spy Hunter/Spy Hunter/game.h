#pragma once
using namespace std;

bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	// http://www.cplusplus.com/reference/set/set/
	
	AvancezLib* engine;
	ObjectPool<Ball> ball_pool;
	GameObject* bounding_box;
	UniformGrid* uniform_grid;
	double ballRadius = 16;

public:

	virtual void Create(AvancezLib* engine)
	{
		SDL_Log("Game::Create");

		this->engine = engine;

		uniform_grid = new UniformGrid();
			   
		ball_pool.Create(4);
		for (auto ball = ball_pool.pool.begin(); ball != ball_pool.pool.end(); ball++)
		{
			BallBehaviourComponent * behaviour = new BallBehaviourComponent();
			behaviour->Create(engine, *ball, &game_objects, &ball_pool);
			RenderComponent * render = new RenderComponent();
			render->Create(engine, *ball, &game_objects, "data/ball.bmp");
			CircleCollideComponent * ball_ball_collision = new CircleCollideComponent();
			ball_ball_collision->Create(engine, *ball, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&ball_pool), ballRadius, uniform_grid);
			RigidBodyComponent * rigidBodyComponent = new RigidBodyComponent();
			rigidBodyComponent->Create(engine, *ball, &game_objects, uniform_grid);
			(*ball)->Create();
			(*ball)->AddComponent(behaviour);
			(*ball)->AddComponent(render);
			(*ball)->AddComponent(ball_ball_collision);
			(*ball)->AddComponent(rigidBodyComponent);
			game_objects.insert(*ball);
		}

		// EDGES
		bounding_box = new GameObject();
		bounding_box->Create();
		bounding_box->position = Vector2D(0, 0);
		BoxCollideComponent* box_collider = new BoxCollideComponent();
		box_collider->Create(engine, bounding_box, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&ball_pool), 640, 480);
		bounding_box->AddComponent(box_collider);
		game_objects.insert(bounding_box);

	}

	virtual void Init()
	{
		uniform_grid->init();

		int cell_index;

		ball_pool.pool[0]->Init(Vector2D(320, 480 - 32));
		ball_pool.pool[0]->GetComponent<RigidBodyComponent*>()->velocity.y = -BALL_SPEED;
		cell_index = uniform_grid->get_cell_index(ball_pool.pool[0]->position);
		uniform_grid->grid[cell_index].push_back(ball_pool.pool[0]);
		

		ball_pool.pool[1]->Init(Vector2D(320, 480 - 150));
		cell_index = uniform_grid->get_cell_index(ball_pool.pool[1]->position);
		uniform_grid->grid[cell_index].push_back(ball_pool.pool[1]);

		ball_pool.pool[2]->Init(Vector2D(300, 480 - 200));
		cell_index = uniform_grid->get_cell_index(ball_pool.pool[2]->position);
		uniform_grid->grid[cell_index].push_back(ball_pool.pool[2]);

		ball_pool.pool[3]->Init(Vector2D(210, 480 - 300));
		cell_index = uniform_grid->get_cell_index(ball_pool.pool[3]->position);
		uniform_grid->grid[cell_index].push_back(ball_pool.pool[3]);

		bounding_box->Init();

		enabled = true;
		id = "Game";
	}

	virtual void Update(float dt)
	{
		AvancezLib::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.esc) {
			Destroy();
			engine->quit();
		}

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);
	}

	virtual void Draw()
	{
		engine->swapBuffers();
		engine->clearWindow();
	}

	virtual void Receive(Message m)
	{
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();
	
		ball_pool.Destroy();
	}
};