#include <Windows.h>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>

#include "Vector2.h"


const float PI = 3.14159f;

const int SCREEN_WIDTH = 120;
const int SCREEN_HEIGHT = 40;
const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;

const float MAX_RENDERING_DISTANCE = 16.0f;

const float PLAYER_WALK_SPEED = 5.0f;
const float PLAYER_ROTATION_SPEED = 1.6f;


Vector2f _playerPos { 9.0f, 12.29f }; // also starting values
float _playerAngle = -PI / 6.0f * 5.0f;
float _playerFOV = PI / 4.0f;


// '#' wall, '.' empty space
const std::wstring map =
L"################"
L"#..............#"
L"#..............#"
L"#..#...........#"
L"#..#.......##..#"
L"#..#.......##..#"
L"#..#...........#"
L"#..#####.......#"
L"#..............#"
L"#......#.......#"
L"#......#.......#"
L"#..............#"
L"#..............#"
L"#..............#"
L"################";


static bool WorldPosHasWall(const Vector2f& worldPos)
{
	Vector2n mapPosToCheck { static_cast<int>(worldPos.X), static_cast<int>(worldPos.Y) };
	return map[mapPosToCheck.Y * MAP_WIDTH + mapPosToCheck.X] == '#';
}

static void HandleInput(float elapsedTime)
{
	Vector2f playerLookDir { sinf(_playerAngle), cosf(_playerAngle) };
	float xWalkAmount = playerLookDir.X * PLAYER_WALK_SPEED * elapsedTime;
	float yWalkAmount = playerLookDir.Y * PLAYER_WALK_SPEED * elapsedTime;

	Vector2f playerNewPos = _playerPos;
	if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		playerNewPos += Vector2f(xWalkAmount, yWalkAmount);
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		playerNewPos -= Vector2f(xWalkAmount, yWalkAmount);

	if (!WorldPosHasWall(playerNewPos))
		_playerPos = playerNewPos;


	float rotationAmount = PLAYER_ROTATION_SPEED * elapsedTime;

	if (GetAsyncKeyState(VK_LEFT))
		_playerAngle -= rotationAmount;
	if (GetAsyncKeyState(VK_RIGHT))
		_playerAngle += rotationAmount;

	_playerAngle = fmod(_playerAngle, PI * 2);
}

static float GetDistanceToWall(const Vector2f& worldPos, float angle)
{
	const float RAY_STEP_VALUE = 0.1f;

	Vector2f lookDir { sinf(angle), cosf(angle) };
	float raycastDistance = 0.0f;

	while (raycastDistance < MAX_RENDERING_DISTANCE)
	{
		raycastDistance += RAY_STEP_VALUE;

		Vector2f pointToCheckForWall
		{
			lookDir.X * raycastDistance + worldPos.X,
			lookDir.Y * raycastDistance + worldPos.Y
		};

		if (pointToCheckForWall.X < 0 || pointToCheckForWall.X >= MAP_WIDTH ||
			pointToCheckForWall.Y < 0 || pointToCheckForWall.Y >= MAP_HEIGHT)
		{
			raycastDistance = MAX_RENDERING_DISTANCE;
			break;
		}
		else
		{
			if (WorldPosHasWall(pointToCheckForWall))
				break;
		}
	}

	return raycastDistance;
}

static int GetScreenCeilingSizeFromDistanceToWall(float distanceToWall)
{
	float screenHalf = SCREEN_HEIGHT / 2.0f;
	int ceilingSize = static_cast<int>(screenHalf - screenHalf / distanceToWall);
	return std::clamp<int>(ceilingSize, 0, static_cast<int>(screenHalf));
}

static wchar_t GetWallShadeFromDistance(float distance)
{
	if (distance <= MAX_RENDERING_DISTANCE / 4.0f)			return 0x2588;	// very close	
	else if (distance < MAX_RENDERING_DISTANCE / 3.0f)		return 0x2593;
	else if (distance < MAX_RENDERING_DISTANCE / 2.0f)		return 0x2592;
	else if (distance < MAX_RENDERING_DISTANCE)				return 0x2591;
	else													return ' ';		// very far away
}

static wchar_t GetFloorShadeFromScreenY(int y)
{
	float screenHalf = SCREEN_HEIGHT / 2.0f;
	float highness = 1.0f - ((y - screenHalf) / screenHalf);
	if (highness < 0.25)		return '#';
	else if (highness < 0.5)	return 'x';
	else if (highness < 0.75)	return '.';
	else if (highness < 0.9)	return '-';
	else						return ' ';
}

static void PrintDebugMessage(wchar_t* screen, float elapsedTime)
{
	swprintf_s(screen, 38, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%6.0f", _playerPos.X, _playerPos.Y, _playerAngle, 1.0f / elapsedTime);
}

int main()
{
	wchar_t* screen = new wchar_t[SCREEN_WIDTH * SCREEN_HEIGHT];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	auto lastFrameTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point thisFrameTime;

	while (true)
	{
		thisFrameTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> elapsedTime = thisFrameTime - lastFrameTime;
		lastFrameTime = thisFrameTime;

		HandleInput(elapsedTime.count());

		for (size_t x = 0; x < SCREEN_WIDTH; x++)
		{
			float rayAngle = (_playerAngle - _playerFOV / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * _playerFOV;
			float distanceToWall = GetDistanceToWall(_playerPos, rayAngle);

			unsigned int ceilingSize = GetScreenCeilingSizeFromDistanceToWall(distanceToWall);
			unsigned int floorSize = SCREEN_HEIGHT - ceilingSize;

			// drawing from left top corner
			for (size_t y = 0; y < SCREEN_HEIGHT; y++)
			{
				size_t screenIndex = y * SCREEN_WIDTH + x;

				if (y <= ceilingSize)
					screen[screenIndex] = ' ';
				else if (y > ceilingSize && y <= floorSize)
					screen[screenIndex] = GetWallShadeFromDistance(distanceToWall);
				else
					screen[screenIndex] = GetFloorShadeFromScreenY(y);
			}
		}

		PrintDebugMessage(screen, elapsedTime.count());

		// print to console
		int screenSize = SCREEN_HEIGHT * SCREEN_WIDTH;
		screen[screenSize - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, screenSize, { 0, 0 }, &dwBytesWritten);
	}
}