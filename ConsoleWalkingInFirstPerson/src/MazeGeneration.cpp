#include "MazeGeneration.h"

#include <vector>
#include <stack>

enum class Direction { Left = 0, Up = 1, Right = 2, Down = 3 };

Vector2n _mazeDimensions;
Vector2n _mapDimensions;

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

static bool InMazeBounds(const Vector2n& pos)
{
	return 0 <= pos.X && pos.X < _mazeDimensions.X && 0 <= pos.Y && pos.Y < _mazeDimensions.Y;
}

static bool Contains(const std::vector<Vector2n> vec, const Vector2n& pos)
{
	return std::find(vec.begin(), vec.end(), pos) != vec.end();
}

static std::vector<Vector2n> GeneratePath()
{
	const int mazeSize = _mazeDimensions.X * _mazeDimensions.Y;
	Vector2n start = (int)(rand() % 2) == 1
						? Vector2n((int)(rand() % _mazeDimensions.X)          , (int)(rand() % 2 * (_mazeDimensions.Y - 1)))
						: Vector2n((int)(rand() % 2 * (_mazeDimensions.X - 1)), (int)(rand() % _mazeDimensions.Y));

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
			availableDirections[i] = !Contains(visited, posToCheck) && InMazeBounds(posToCheck);
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

static Vector2n MazePosToMapPos(const Vector2n& mazePosition)
{
	return { mazePosition.X * 2 + 1, mazePosition.Y * 2 + 1 };
}

static int MazePosToMapIndex(const Vector2n& mazePosition)
{
	Vector2n mazePos = MazePosToMapPos(mazePosition);
	return mazePos.X * _mapDimensions.X + mazePos.Y;
}

static Vector2n PointBetween(const Vector2n& lhs, const Vector2n& rhs)
{
	return { (lhs.X + rhs.X) / 2, (lhs.Y + rhs.Y) / 2 };
}

static std::wstring GenerateMap(const std::vector<Vector2n>& path)
{
	std::wstring map(_mapDimensions.X * _mapDimensions.Y, '#');

	std::stack<Vector2n> pathToDraw;
	pathToDraw.push(path[0]);
	map[MazePosToMapIndex(path[0])] = '.';

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
			auto pointBetween = PointBetween(MazePosToMapPos(thisPoint), MazePosToMapPos(nextPoint));
			map[pointBetween.Y * _mapDimensions.X + pointBetween.X] = '.';
			map[MazePosToMapIndex(nextPoint)] = '.';

			pathToDraw.push(nextPoint);
		}
	}

	return map;
}

static std::wstring GenerateMaze(const int width, const int height)
{
	_mazeDimensions = { width, height };
	_mapDimensions = { width * 2 + 1, height * 2 + 1 };

	std::vector<Vector2n> path(GeneratePath());
	std::wstring map = GenerateMap(path);

	// add exit
	const Vector2n& startMazePoint = path[0];
	Vector2n startMapPoint = MazePosToMapPos(path[0]);
	if (startMazePoint.X == 0)
		startMapPoint += DirectionToVector2n(Direction::Left);
	else if (startMazePoint.Y == 0)
		startMapPoint += DirectionToVector2n(Direction::Down);
	else if (startMazePoint.X == (width - 1))
		startMapPoint += DirectionToVector2n(Direction::Right);
	else if (startMazePoint.Y == (height - 1))
		startMapPoint += DirectionToVector2n(Direction::Up);
	map[startMapPoint.Y * _mapDimensions.X + startMapPoint.X] = '.';

	return map;
}

Maze::Maze(const int width, const int height)
	: WIDTH(width * 2 + 1), HEIGHT(height * 2 + 1), map(GenerateMaze(width, height))
{}

Maze::~Maze() {}

int Maze::GetWidth() const { return WIDTH; }

int Maze::GetHeight() const { return HEIGHT; }

const std::wstring& Maze::GetMap() const { return map; }

void Maze::Regenerate()
{
	map = GenerateMaze(WIDTH, HEIGHT);
}