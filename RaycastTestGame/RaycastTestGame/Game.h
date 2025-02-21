#pragma once

#include <vector>

#include "Texture.h"

class Viewport;
class Player;
class Camera;
class Map;

using std::vector;

class Game
{
public:
	Game();
	int Run();

public:
	float deltaTime;

private:
	void Raycast(Viewport*& viewport, Player*& player, Camera*& camera, Map*& map, vector<Texture*> textures, bool useASCII);
	void GetAsyncKeyboardInput(Player*& player, Camera*& camera, Map*& map);
	void CreateDefaultTextures(vector<Texture*>& textureList, Vector2i textureSize);

	Color GetColorFromRaycast(int x, int y, Map*& map, bool isHorizontal);
	unsigned char GetASCIIColorFromRaycast(int x, int y, Map*& map, bool isHorizontal);

private:
	float m_time;
	float m_oldTime;

	bool gameIsRunning;
};

