#pragma once

#include <vector>
#include <string>

#include "Texture.h"

class Viewport;
class Player;
class Camera;
class Map;

using std::vector;
using std::string;

class Game
{
public:
	Game();
	int Run();

public:
	float deltaTime;

private:
	void Raycaster(Viewport*& viewport, Player*& player, Camera*& camera, Map*& map, vector<Texture*> textures, bool useASCII);
	void OldKeyboardInput(Player*& player, Camera*& camera, Map*& map);
	void KeyboardInput(Player*& player, Camera*& camera, Map*& map);
	void CommandInput(string command, Player*& player, Camera*& camera, Map*& map);
	void CreateDefaultTextures(vector<Texture*>& textureList, Vector2i textureSize);

private:
	float m_time;
	float m_oldTime;

	Player* m_player;

	bool gameIsRunning;
};

