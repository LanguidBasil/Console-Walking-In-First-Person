#pragma once

#include <string>

class Maze
{
public:
	Maze(const int width, const int height);
	~Maze();

	int GetWidth() const;
	int GetHeight() const;
	const std::wstring& GetMap() const;

	void Regenerate();

private:
	const int WIDTH, HEIGHT;
	// # - wall, . - empty space
	std::wstring map;
};