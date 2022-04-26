#include "Maze.h"

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

static bool InMazeBounds(const Vector2n& pos, const int width, const int height)
{
	return 0 <= pos.X && pos.X < width && 0 <= pos.Y && pos.Y < height;
}

static bool Contains(const std::vector<Vector2n> vec, const Vector2n& pos)
{
	return std::find(vec.begin(), vec.end(), pos) != vec.end();
}

static Vector2n MazePosToMapPos(const Vector2n& mazePosition)
{
	return { mazePosition.X * 2 + 1, mazePosition.Y * 2 + 1 };
}

static int MazePosToMapIndex(const Vector2n& mazePosition, const int width)
{
	Vector2n mazePos = MazePosToMapPos(mazePosition);
	return mazePos.X * width + mazePos.Y;
}

static Vector2n PointBetween(const Vector2n& lhs, const Vector2n& rhs)
{
	return { (lhs.X + rhs.X) / 2, (lhs.Y + rhs.Y) / 2 };
}


Maze::Maze()
	:	MAZE_WIDTH(0), MAZE_HEIGHT(0), MAP_WIDTH(0), MAP_HEIGHT(0), 
		_mazePath(), _map(),
		_endMapPosition(), _startMapPosition()
{}

Maze::~Maze() {}

void Maze::Generate(const int width, const int height)
{
	MAZE_WIDTH = width; MAZE_HEIGHT = height;
	MAP_WIDTH = width * 2 + 1;  MAP_HEIGHT = height * 2 + 1;

	_mazePath = _GenerateMazePath();
	_map = _GenerateMap();
	_endMapPosition = _GenerateMapEndPosition();
	_startMapPosition = _GenerateMapStartPosition();
	
	_map[_endMapPosition.Y * MAP_WIDTH + _endMapPosition.X] = '.';
}

std::vector<Vector2n> Maze::_GenerateMazePath()
{
	const int mazeSize = MAZE_WIDTH * MAZE_HEIGHT;
	Vector2n start = (int)(rand() % 2) == 1
		? Vector2n((int)(rand() % MAZE_WIDTH), (int)(rand() % 2 * (MAZE_HEIGHT - 1)))
		: Vector2n((int)(rand() % 2 * (MAZE_WIDTH - 1)), (int)(rand() % MAZE_HEIGHT));

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
			availableDirections[i] = !Contains(visited, posToCheck) && InMazeBounds(posToCheck, MAZE_WIDTH, MAZE_HEIGHT);
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

std::wstring Maze::_GenerateMap()
{
	std::wstring map(MAP_WIDTH * MAP_HEIGHT, '#');

	std::stack<Vector2n> pathToDraw;
	pathToDraw.push(_mazePath[0]);
	map[MazePosToMapIndex(_mazePath[0], MAP_WIDTH)] = '.';

	for (size_t i = 1; i < _mazePath.size(); i++)
	{
		const Vector2n& nextPoint = _mazePath[i];
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
			map[pointBetween.Y * MAP_WIDTH + pointBetween.X] = '.';
			map[MazePosToMapIndex(nextPoint, MAP_WIDTH)] = '.';

			pathToDraw.push(nextPoint);
		}
	}

	return map;
}

Vector2n Maze::_GenerateMapEndPosition()
{
	const Vector2n& startMazePoint = _mazePath[0];
	Vector2n endMapPosition = MazePosToMapPos(_mazePath[0]);

	if (startMazePoint.X == 0)
		endMapPosition += DirectionToVector2n(Direction::Left);
	else if (startMazePoint.Y == 0)
		endMapPosition += DirectionToVector2n(Direction::Down);
	else if (startMazePoint.X == (MAZE_WIDTH - 1))
		endMapPosition += DirectionToVector2n(Direction::Right);
	else if (startMazePoint.Y == (MAZE_HEIGHT - 1))
		endMapPosition += DirectionToVector2n(Direction::Up);

	return endMapPosition;
}

Vector2n Maze::_GenerateMapStartPosition()
{
	Vector2n mazeMiddlePoint = { MAZE_WIDTH / 2, MAZE_HEIGHT / 2 };

	int xInArea = (int)rand() % mazeMiddlePoint.X;
	int yInArea = (int)rand() % mazeMiddlePoint.Y;

	Vector2n startPos	{ xInArea + (mazeMiddlePoint.X * _endMapPosition.X < mazeMiddlePoint.X ? 1 : 0),
						  yInArea + (mazeMiddlePoint.Y * _endMapPosition.Y < mazeMiddlePoint.Y ? 1 : 0)};

	return MazePosToMapPos(startPos);
}

int Maze::GetMazeWidth() const { return MAZE_WIDTH; }
int Maze::GetMazeHeight() const { return MAZE_HEIGHT; }
const std::vector<Vector2n>& Maze::GetMazePath() const { return _mazePath; }

int Maze::GetMapWidth() const { return MAP_WIDTH; }
int Maze::GetMapHeight() const { return MAP_HEIGHT; }
const std::wstring& Maze::GetMap() const { return _map; }

Vector2n Maze::GetStartPos() const { return _startMapPosition; }
Vector2n Maze::GetExitPos() const { return _endMapPosition; }