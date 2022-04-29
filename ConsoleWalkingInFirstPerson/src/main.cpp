#include <Windows.h>
#include <string>
#include <string.h>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iostream>

#include "Vector2.h"
#include "Maze.h"


const float PI = 3.14159f;

const Vector2n SCREEN_DIMENSIONS { 120, 40 };
const Vector2n MAZE_DIMENSIONS { 6, 6 };

const float MAX_RENDERING_DISTANCE = 16.0f;

const float PLAYER_WALK_SPEED = 3.0f;
const float PLAYER_ROTATION_SPEED = 1.6f;


Vector2f _playerPos;
float _playerAngle;
float _playerFOV;

bool _wantToPlay; 
bool _gameOver;
bool _mapIsVisible;
bool _inDebug;


static bool WorldPosHasWall(const std::wstring& map, const Vector2n& mapDimensions, const Vector2f& worldPos)
{
	Vector2n mapPosToCheck { (int)worldPos.X, (int)worldPos.Y };
	if (0 <= mapPosToCheck.X && mapPosToCheck.X < mapDimensions.X && 
		0 <= mapPosToCheck.Y && mapPosToCheck.Y < mapDimensions.Y)
		return map[mapPosToCheck.Y * mapDimensions.X + mapPosToCheck.X] == '#';
	else
		return false;
}

static void HandleMenuInput()
{
	while (!(GetAsyncKeyState(VK_RETURN) & 0x0001)) {}
}

static void HandleInput(const std::wstring& map, const Vector2n& mapDimensions, float elapsedTime)
{
	float walkAmount = PLAYER_WALK_SPEED * elapsedTime;

	Vector2f playerLookDir { cosf(_playerAngle), sinf(_playerAngle) };
	Vector2f forwardsMoveAmount { playerLookDir.X * walkAmount, playerLookDir.Y * walkAmount };

	Vector2f playerRightDir { cosf(_playerAngle + PI / 2), sinf(_playerAngle + PI / 2) };
	Vector2f sidewaysMoveAmount { playerRightDir.X * walkAmount, playerRightDir.Y * walkAmount };

	Vector2f playerNewPos = _playerPos;

	if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		playerNewPos += forwardsMoveAmount;
	if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		playerNewPos -= forwardsMoveAmount;

	if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		playerNewPos += sidewaysMoveAmount;
	if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		playerNewPos -= sidewaysMoveAmount;

	if (!WorldPosHasWall(map, mapDimensions, playerNewPos))
		_playerPos = playerNewPos;


	float rotationAmount = PLAYER_ROTATION_SPEED * elapsedTime;

	if (GetAsyncKeyState(VK_LEFT))
		_playerAngle -= rotationAmount;
	if (GetAsyncKeyState(VK_RIGHT))
		_playerAngle += rotationAmount;

	_playerAngle = fmod(_playerAngle, PI * 2);

	if (GetAsyncKeyState((unsigned short)'M') & 0x0001)
		_mapIsVisible = !_mapIsVisible;
	if (GetAsyncKeyState(VK_DELETE) & 0x0001)
		_inDebug = !_inDebug;
}

static bool HandleGameOverInput()
{
	while (true)
	{
		// VK_RETURN is 'enter' key
		if (GetAsyncKeyState(VK_RETURN) & 0x0001)
			return true;
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			return false;
	}
}

static float GetDistanceToWall(const std::wstring& map, const Vector2n& mapDimensions, const Vector2f& worldPos, float angle)
{
	const float RAY_STEP_VALUE = 0.1f;

	Vector2f lookDir { cosf(angle), sinf(angle) };
	float raycastDistance = 0.0f;

	while (raycastDistance < MAX_RENDERING_DISTANCE)
	{
		raycastDistance += RAY_STEP_VALUE;

		Vector2f pointToCheckForWall
		{
			lookDir.X * raycastDistance + worldPos.X,
			lookDir.Y * raycastDistance + worldPos.Y
		};

		if (WorldPosHasWall(map, mapDimensions, pointToCheckForWall))
			break;
	}

	return raycastDistance;
}

static int GetScreenCeilingSizeFromDistanceToWall(float distanceToWall)
{
	float screenHalf = SCREEN_DIMENSIONS.Y / 2.0f;
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
	float screenHalf = SCREEN_DIMENSIONS.Y / 2.0f;
	float highness = 1.0f - ((y - screenHalf) / screenHalf);
	if (highness < 0.25)		return '#';
	else if (highness < 0.5)	return 'x';
	else if (highness < 0.75)	return '.';
	else if (highness < 0.9)	return '-';
	else						return ' ';
}

static float GetNormalizedDistanceToEnd(const Vector2n& endPosition, const Vector2n& mapDimensions)
{
	const Vector2n mapDimWithoutWalls { mapDimensions.X - 2, mapDimensions.Y - 2 };
	const float maxDistance = abs(mapDimWithoutWalls.X) + abs(mapDimWithoutWalls.Y);

	const Vector2n difference { abs(endPosition.X - (int)_playerPos.X), abs(endPosition.Y - (int)_playerPos.Y)};
	return ((difference.X + difference.Y) / maxDistance);
}


static void WriteColumn(wchar_t* screen, const int x, const std::wstring& map, const Vector2n& mapDimensions)
{
	float rayAngle = (_playerAngle - _playerFOV / 2.0f) + ((float)x / (float)SCREEN_DIMENSIONS.X) * _playerFOV;
	float distanceToWall = GetDistanceToWall(map, mapDimensions, _playerPos, rayAngle);

	unsigned int ceilingSize = GetScreenCeilingSizeFromDistanceToWall(distanceToWall);
	unsigned int floorSize = SCREEN_DIMENSIONS.Y - ceilingSize;

	// drawing from left top corner
	for (size_t y = 0; y < SCREEN_DIMENSIONS.Y; y++)
	{
		size_t screenIndex = y * SCREEN_DIMENSIONS.X + x;

		if (y <= ceilingSize)
			screen[screenIndex] = ' ';
		else if (y > ceilingSize && y <= floorSize)
			screen[screenIndex] = GetWallShadeFromDistance(distanceToWall);
		else
			screen[screenIndex] = GetFloorShadeFromScreenY(y);
	}
}

static void WriteProgressToEnd(wchar_t* screen, int screenYOffset, const float distanceToEnd)
{
	const wchar_t* message;
	if (distanceToEnd < 0.05)
		message = L"!!!!!!!!\0";
	else if (0.05f  <= distanceToEnd && distanceToEnd < 0.125f)
		message = L"########\0";
	else if (0.125f <= distanceToEnd && distanceToEnd < 0.25f)
		message = L"#######-\0";
	else if (0.25f  <= distanceToEnd && distanceToEnd < 0.375f)
		message = L"######--\0";
	else if (0.375f <= distanceToEnd && distanceToEnd < 0.5f)
		message = L"#####---\0";
	else if (0.5f   <= distanceToEnd && distanceToEnd < 0.625f)
		message = L"####----\0";
	else if (0.625f <= distanceToEnd && distanceToEnd < 0.75f)
		message = L"###-----\0";
	else if (0.75f  <= distanceToEnd && distanceToEnd < 0.875f)
		message = L"##------\0";
	else if (0.875f <= distanceToEnd && distanceToEnd < 0.95f)
		message = L"#-------\0";
	else
		message = L"--------\0";

	for (size_t i = 0; i < wcslen(message); i++)
		screen[screenYOffset * SCREEN_DIMENSIONS.X + i] = message[i];
}

static void WriteMap(wchar_t* screen, int screenYOffset, const std::wstring& map, const Vector2n& mapDimensions)
{
	for (size_t y = 0; y < mapDimensions.Y; y++)
		for (size_t x = 0; x < mapDimensions.X; x++)
			screen[(y + screenYOffset) * SCREEN_DIMENSIONS.X + x] = map[y * mapDimensions.X + x];
	screen[((int)_playerPos.Y + screenYOffset) * SCREEN_DIMENSIONS.X + (int)_playerPos.X] = L'P';
}

static void WriteGameOver(wchar_t* screen)
{
	auto message = L"You won!";
	for (size_t i = 0; i < wcslen(message); i++)
		screen[i + SCREEN_DIMENSIONS.X * 0] = message[i];

	message = L"If you want to try again press enter";
	for (size_t i = 0; i < wcslen(message); i++)
		screen[i + SCREEN_DIMENSIONS.X * 1] = message[i];

	message = L"If you want to exit press escape";
	for (size_t i = 0; i < wcslen(message); i++)
		screen[i + SCREEN_DIMENSIONS.X * 2] = message[i];
}

static void WriteDebugMessage(wchar_t* screen, int screenYOffset, float elapsedTime, float distanceToEnd)
{
	wchar_t message[45];
	swprintf(message, 45, L"X=%3.2f, Y=%3.2f, A=%3.2f, DtE=%1.2f, FPS=%5.0f\0",
		_playerPos.X, _playerPos.Y, _playerAngle, distanceToEnd, 1.0f / elapsedTime);

	for (size_t i = 0; i < wcslen(message); i++)
		screen[screenYOffset * SCREEN_DIMENSIONS.X + i] = message[i];
}

static void Print(wchar_t* screen, HANDLE consoleHandle)
{
	int screenSize = SCREEN_DIMENSIONS.X * SCREEN_DIMENSIONS.Y;
	screen[screenSize - 1] = '\0';

	DWORD _;
	WriteConsoleOutputCharacter(consoleHandle, screen, screenSize, { 0, 0 }, &_);
}

static void WriteStartMenu(wchar_t* screen)
{
	auto message =
		LR"(.__                            .___   __           .__                                                                  )"
		LR"(|__|  __ __  ______  ____    __| _/ _/  |_  ____   |  |    ____ ___  __  ____     _____  _____   ________  ____    _____)"
		LR"(|  | |  |  \/  ___/_/ __ \  / __ |  \   __\/  _ \  |  |   /  _ \\  \/ / / __ \   /     \ \__  \  \___   /_/ __ \  /  ___)"
		LR"(|  | |  |  /\___ \ \  ___/ / /_/ |   |  | (  <_> ) |  |__(  <_> )\   / |  ___/  |  Y Y  \ / __ \_ /    / \  ___/  \___ \)"
		LR"(|__| |____//____  > \___  >\____ |   |__|  \____/  |____/ \____/  \_/   \___  > |__|_|  /(____  //_____ \ \___  >/____  )"
		LR"(                \/      \/      \/                                          \/        \/      \/       \/     \/      \/)"
		LR"(                                                                                                                        )"
		LR"(      ____ _  _ ____     __  ____ ____ ____ ____    ____ __ _ ____ ____ ____ __ __ _  ___    ____ _  _ __ ____          )"
		LR"(     (  _ / )( (_  _)   / _\(  __(_  _(  __(  _ \  (  __(  ( (_  _(  __(  _ (  (  ( \/ __)  (_  _/ )( (  / ___)         )"
		LR"(      ) _ ) \/ ( )(    /    \) _)  )(  ) _) )   /   ) _)/    / )(  ) _) )   /)(/    ( (_ \    )( ) __ ()(\___ \         )"
		LR"(     (____\____/(__)   \_/\_(__)  (__)(____(__\_)  (____\_)__)(__)(____(__\_(__\_)__)\___/   (__)\_)(_(__(____/         )"
		LR"(      __    __  ____ ____     __  __ _ ____      __ __  _    __ _  __ ____    ____  __     ____ _  _ ____ ____          )"
		LR"(     (  )  / _\/ ___(_  _)   /  \(  ( (  __)_   (  (( \/ )  (  ( \/  (_  _)  / ___)/  \   / ___/ )( (  _ (  __)         )"
		LR"(     / (_//    \___ \ )(    (  O /    /) _)( )   )( / \/ \  /    (  O ))(    \___ (  O )  \___ ) \/ ()   /) _)          )"
		LR"(     \____\_/\_(____/(__)    \__/\_)__(____(/   (__)\_)(_/  \_)__)\__/(__)   (____/\__/   (____\____(__\_(____)         )"
		LR"(      _  _ _  _ ____ ____ ____    __    ____ ____ __  __ _ ____     __  __ _ _  _ _  _  __ ____ ____                    )"
		LR"(     / )( / )( (  __(  _ (  __)  (  )  / ___(_  _/ _\(  ( (    \   / _\(  ( ( \/ ( \/ )/  (  _ (  __)                   )"
		LR"(     \ /\ ) __ () _) )   /) _)    )(   \___ \ )(/    /    /) D (  /    /    /)  // \/ (  O )   /) _) _                  )"
		LR"(     (_/\_\_)(_(____(__\_(____)  (__)  (____/(__\_/\_\_)__(____/  \_/\_\_)__(__/ \_)(_/\__(__\_(____(_)                 )"
		LR"(                                                                                                                        )"
		LR"(                                                                                                                        )"
		LR"(                                                    controls                                                            )"
		LR"(                                    wasd - to move            arrows - to look                                          )"
		LR"(                                                                                                                        )"
		LR"(                                                                                                                        )"
		LR"(                                               press enter to start                                                     )"
		LR"(                                                                                                                        )"
		LR"(                                                                                                                        )";

	auto messageLength = wcslen(message);
	for (size_t i = 0; i < wcslen(screen); i++)
		screen[i] = i < messageLength ? message[i] : ' ';
}


static void ConsoleInit(wchar_t*& screen, HANDLE& consoleHandle)
{
	srand(time(NULL));

	screen = new wchar_t[SCREEN_DIMENSIONS.X * SCREEN_DIMENSIONS.Y];
	consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS);

	// set console font
	CONSOLE_FONT_INFOEX fontex;
	fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(consoleHandle, 0, &fontex);
	fontex.dwFontSize.Y = 16;
	SetCurrentConsoleFontEx(consoleHandle, NULL, &fontex);

	SetConsoleActiveScreenBuffer(consoleHandle);
}

static void GameMenu(wchar_t* screen, HANDLE consoleHandle)
{
	WriteStartMenu(screen);
	Print(screen, consoleHandle);
	HandleMenuInput();
}

static void GameInit(Maze& maze, std::wstring& map, Vector2n& mapDim, Vector2n& endPos)
{
	maze.Generate(MAZE_DIMENSIONS.X, MAZE_DIMENSIONS.Y);

	_playerAngle = -PI / 2;
	_playerFOV = PI / 4.0f;

	_gameOver = false;
	_mapIsVisible = false;
	_inDebug = false;

	_playerPos = Vector2f(maze.GetStartPos()) + Vector2f(0.5f, 0.5f);

	map = maze.GetMap();
	mapDim = { maze.GetMapWidth(), maze.GetMapHeight() };
	endPos = maze.GetExitPos();
}

static void GameStart(wchar_t* screen, HANDLE consoleHandle)
{
	_wantToPlay = true;


	auto lastFrameTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point thisFrameTime;

	Maze maze;
	const std::wstring map;
	const Vector2n mapDim;
	const Vector2n endPos;

	while (_wantToPlay)
	{
		GameInit(maze, const_cast<std::wstring&>(map), const_cast<Vector2n&>(mapDim), const_cast<Vector2n&>(endPos));

		while (!_gameOver)
		{
			thisFrameTime = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsedTime = thisFrameTime - lastFrameTime;
			lastFrameTime = thisFrameTime;

			HandleInput(map, mapDim, elapsedTime.count());

			for (size_t x = 0; x < SCREEN_DIMENSIONS.X; x++)
				WriteColumn(screen, x, map, mapDim);

			float distanceToEnd = GetNormalizedDistanceToEnd(endPos, mapDim);
			_gameOver = distanceToEnd < 0.01f;

			WriteProgressToEnd(screen, 0, distanceToEnd);
			if (_mapIsVisible)
				WriteMap(screen, 1, map, mapDim);
			if (_inDebug)
				WriteDebugMessage(screen, SCREEN_DIMENSIONS.Y - 1, elapsedTime.count(), distanceToEnd);

			Print(screen, consoleHandle);
		}

		WriteGameOver(screen);
		Print(screen, consoleHandle);
		_wantToPlay = HandleGameOverInput();
	}
}


int main()
{
	wchar_t* screen = nullptr; HANDLE consoleHandle;
	ConsoleInit(screen, consoleHandle);

	GameMenu(screen, consoleHandle);
	GameStart(screen, consoleHandle);
}