#pragma once

#include <string>
#include "Vector2.h"

// # - wall, . - empty space
std::wstring GenerateMaze(const int width, const int height);

Vector2n CalculateMapDimensions(const int width, const int height);