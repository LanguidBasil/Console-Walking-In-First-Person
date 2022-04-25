#pragma once

#include <string>
#include <vector>

#include "Vector2.h"

class Maze
{
public:
	Maze(const int width, const int height);
	~Maze();

	int GetMazeWidth() const;
	int GetMazeHeight() const;
	const std::vector<Vector2n>& GetMazePath() const;

	int GetMapWidth() const;
	int GetMapHeight() const;
	const std::wstring& GetMap() const;

	Vector2n GetStartPos() const;
	Vector2n GetExitPos() const;

private:
	const int MAZE_WIDTH, MAZE_HEIGHT;
	std::vector<Vector2n> _mazePath;

	// # - wall, . - empty space
	const int MAP_WIDTH, MAP_HEIGHT;
	std::wstring _map;

	Vector2n _startMapPosition;
	Vector2n _endMapPosition;

	std::vector<Vector2n> _GenerateMazePath();
	std::wstring _GenerateMap();
	Vector2n _GenerateMapStartPosition();
	Vector2n _GenerateMapEndPosition();
};