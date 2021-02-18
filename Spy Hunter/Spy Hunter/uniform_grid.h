#pragma once
#include <unordered_map>
using namespace std;

class UniformGrid
{
public:

	const int	BALL_RADIUS = 16;
	const int	SCREEN_HEIGHT = 480;
	const int	SCREEN_WIDTH = 640;
	
	const int	CELL_SIZE = BALL_RADIUS * 2;
	const int	GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
	const int	GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;
	const int	GRID_SIZE = GRID_WIDTH * GRID_HEIGHT;

	//vector <GameObject*> grid[GRID_SIZE] = {};
	unordered_map <int, vector <GameObject*>> grid;

	int get_cell_index(Vector2D position)
	{
		int row = floor(position.x / CELL_SIZE);
		int col = floor(position.y / CELL_SIZE);
		int cell_index = row + (col * GRID_WIDTH);
		return cell_index;
	}

	void init()
	{
		for (int i = 0; i < GRID_SIZE; ++i)
		{
			grid[i] = vector<GameObject*>();
		}

	}

	void get_neighbours(int cell_index, set<GameObject*>* balls)
	{

		int row_min = -1;

		if (((cell_index - 1) % GRID_WIDTH) > (cell_index % GRID_WIDTH))
		{
			row_min = 0; // if you are at the left edge of the grid, no neighbour to the left
		}
		
		int row_max = 2;
		
		if (((cell_index + 1) % GRID_WIDTH) < (cell_index % GRID_WIDTH))
		{
			row_max = 1; // if you are at the right edge of the grid, no neighbour to the right
		}

		for (int row_offset = row_min; row_offset < row_max; ++row_offset)
		{

			for (int col_offset = -1; col_offset < 2; ++col_offset)
			{

				int index = cell_index + row_offset + (col_offset * GRID_WIDTH);

				if (0 <= index < GRID_SIZE && grid[index].size() > 0)
				{
					
					for (auto ball = grid[index].begin(); ball != grid[index].end(); ++ball)
					{
						balls->insert(*ball);
					}
				}
			}
		}
	}

	// move_ball from old cell to new cell
	void move_ball(GameObject* ball, int old_index, int new_index)
	{
		if (0 <= new_index < GRID_SIZE) {
			
auto it = grid[old_index].begin();
			while (it != grid[old_index].end())
			{
				if ((*it) == ball)
				{
					it = grid[old_index].erase(it);
				}
				else
				{
					++it;
				}
			}
			grid[new_index].push_back(ball);
		}
		return;
	}
};