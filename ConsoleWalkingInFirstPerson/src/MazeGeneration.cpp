#include "MazeGeneration.h"

#include <vector>
#include <stack>

enum class Direction { Left = 0, Up = 1, Right = 2, Down = 3 };

static Vector2n DirectionToVector2n(const Direction dir)
{
	switch (dir)
	{
	case Direction::Left:	return Vector2n(-1, 0);
	case Direction::Up:		return Vector2n(0, 1);
	case Direction::Right:	return Vector2n(1, 0);
	case Direction::Down:	return Vector2n(0, -1);
	}
}

static bool InBounds(const Vector2n& pos, const int width, const int height)
{
	return 0 <= pos.X && pos.X < width && 0 <= pos.Y && pos.Y < height;
}

static bool Contains(const std::vector<Vector2n> vec, const Vector2n& pos)
{
	return std::find(vec.begin(), vec.end(), pos) != vec.end();
}

static std::vector<Vector2n> GeneratePath(const int width, const int height)
{
	const int mazeSize = width * height;
	Vector2n start = (int)(rand() % 2) == 1
						? Vector2n((int)(rand() % width), (int)(rand() % 2 * height - 1))
						: Vector2n((int)(rand() % 2 * width - 1), (int)(rand() % height));

	std::vector<Vector2n> visited(mazeSize, { -1, -1 });
	int visitedCount = 0;
	visited[visitedCount++] = start;

	std::stack<Vector2n> breadcrumbs;
	breadcrumbs.push(start);


	Vector2n currentPos = start;
	bool availableDirections[4];

	while (visitedCount < mazeSize)
	{
		for (size_t i = 0; i < 4; i++)
		{
			auto posToCheck = currentPos + DirectionToVector2n((Direction)i);
			availableDirections[i] = !Contains(visited, posToCheck) && InBounds(posToCheck, width, height);
		}

		std::vector<Direction> dirs;
		for (size_t i = 0; i < 4; i++)
			if (availableDirections[i])
				dirs.push_back((Direction)i);

		if (dirs.size() != 0)
		{
			Direction dir = dirs[(int)(rand() % dirs.size())];
			currentPos += DirectionToVector2n(dir);
			visited[visitedCount++] = currentPos;
			breadcrumbs.push(currentPos);
		}
		else
		{
			breadcrumbs.pop();
			currentPos = breadcrumbs.top();
		}
	}

	return visited;
}

Vector2n CalculateMapDimensions(const int width, const int height)
{
	return { width * 2 + 1, height * 2 + 1 };
}

static std::wstring GenerateMap(const std::vector<Vector2n> path, const int width, const int height)
{
	const Vector2n MAP_DIM = CalculateMapDimensions(width, height);
	std::wstring map(MAP_DIM.X * MAP_DIM.Y, '#');

	return map;
}

std::wstring GenerateMaze(const int width, const int height)
{
	std::vector<Vector2n> path(GeneratePath(width, height));
	return GenerateMap(path, width, height);
}