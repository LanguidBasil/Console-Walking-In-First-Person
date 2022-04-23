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
						? Vector2n((int)(rand() % width)          , (int)(rand() % 2 * (height - 1)))
						: Vector2n((int)(rand() % 2 * (width - 1)), (int)(rand() % height));

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

static Vector2n MazePosToMapPos(const Vector2n& mazePosition, const int width, const int height)
{
	return { mazePosition.X * 2 + 1, mazePosition.Y * 2 + 1 };
}

static int MazePosToMapIndex(const Vector2n& mazePosition, const int width, const int height)
{
	Vector2n mazePos = MazePosToMapPos(mazePosition, width, height);
	return mazePos.X * width + mazePos.Y;
}

static Vector2n PointBetween(const Vector2n& lhs, const Vector2n& rhs)
{
	return { (lhs.X + rhs.X) / 2, (lhs.Y + rhs.Y) / 2 };
}

static std::wstring GenerateMap(const std::vector<Vector2n>& path, const int width, const int height)
{
	const Vector2n MAP_DIM = CalculateMapDimensions(width, height);
	std::wstring map(MAP_DIM.X * MAP_DIM.Y, '#');

	std::stack<Vector2n> pathToDraw;
	pathToDraw.push(path[0]);
	map[MazePosToMapIndex(path[0], MAP_DIM.X, MAP_DIM.Y)] = '.';

	for (size_t i = 1; i < path.size(); i++)
	{
		const Vector2n& nextPoint = path[i];
		const Vector2n& thisPoint = pathToDraw.top();
		Vector2n difference = Vector2n::Abs(thisPoint - nextPoint);
		if ((difference.X + difference.Y) > 1)
		{
			pathToDraw.pop();
			i--;
		}
		else
		{
			auto pointBetween = PointBetween(MazePosToMapPos(thisPoint, width, height), MazePosToMapPos(nextPoint, width, height));
			map[pointBetween.Y * MAP_DIM.X + pointBetween.X] = '.';
			map[MazePosToMapIndex(nextPoint, MAP_DIM.X, MAP_DIM.Y)] = '.';

			pathToDraw.push(nextPoint);
		}
	}

	return map;
}

Vector2n CalculateMapDimensions(const int width, const int height)
{
	return { width * 2 + 1, height * 2 + 1 };
}

std::wstring GenerateMaze(const int width, const int height)
{
	std::vector<Vector2n> path(GeneratePath(width, height));
	return GenerateMap(path, width, height);
}