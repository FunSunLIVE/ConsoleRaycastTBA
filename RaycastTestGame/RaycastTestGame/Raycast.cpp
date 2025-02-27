#include "Raycast.h"

#include "Color.h";
#include "Map.h";
#include "Player.h";
#include "Texture.h";
#include "Viewport.h";

void FloorRaycast(int y, Viewport*& viewport, Player*& player, Camera*& camera, Map*& map, vector<Texture*> textures)
{

	float& plPosX = player->position.x;
	float& plPosY = player->position.y;

	float& camSizeX = camera->size.x;
	float& camSizeY = camera->size.y;

	float& plDirX = player->direction.x;
	float& plDirY = player->direction.y;

	int& width = viewport->size.x;
	int& height = viewport->size.y;

	// Ray direction for leftmost ray (x = 0) and righmost ( x = width)
	Vector2 l_rayDir =
	{
		plDirX - camSizeX,
		plDirY - camSizeY
	};

	Vector2 r_rayDir =
	{
		plDirX + camSizeX,
		plDirY + camSizeY
	};

	// Current y position compared to the center of viewport (midpoint / horizon)
	int p = y - height * 0.5;

	// Z position of the camera (the centre of the screen)
	float z = 0.5 * height;

	// 0.0 - 1.0 ratio of the z position. 0.5 is the centre of the screen
	float rowDist = z / p;

	// calculate step vector we have to add for screen x pos (parallel to camera plane)
	Vector2 floorStep =
	{
		rowDist * (r_rayDir.x - l_rayDir.x) / width,
		rowDist * (r_rayDir.y - l_rayDir.y) / width,
	};

	// coordinates of the leftmost column (x = 0) on screen. Gets updated as we move right
	Vector2 floorPos =
	{
		plPosX + rowDist * l_rayDir.x,
		plPosY + rowDist * l_rayDir.y
	};

	for (int x = 0; x < width; x++)
	{
		// the map cell pos
		Vector2i mapPos = { (int)floorPos.x, (int)floorPos.y };

		Texture* floorTexture = textures[8];

		Vector2i floorTextureSize = floorTexture->GetSize();

		// Texture pos from fractional part
		Vector2i floorTexturePos =
		{
			// (int)(texWidth * (floorX - cellX)) & (texWidth - 1)
			((int)(floorTextureSize.x * (floorPos.x - mapPos.x))) & (floorTextureSize.x - 1),
			((int)(floorTextureSize.y * (floorPos.y - mapPos.y))) & (floorTextureSize.y - 1)
		};

		Texture* ceilTexture = textures[8];

		Vector2i ceilTextureSize = ceilTexture->GetSize();

		Vector2i ceilTexturePos = {
		((int)(ceilTextureSize.x * (floorPos.x - mapPos.x))) & (ceilTextureSize.x - 1),
		((int)(ceilTextureSize.y * (floorPos.y - mapPos.y))) & (ceilTextureSize.y - 1)
		};

		// Update floor and ceiling positions
		floorPos.x += floorStep.x;
		floorPos.y += floorStep.y;


		Color color;

		// Drawing floor
		Color floorColor = floorTexture->GetColorFromLocation(floorTexturePos.x, floorTexturePos.y);
		floorColor /= 1.25f; // Dim the color
		viewport->AddColorToBuffer(x, y, floorColor);
		
		// Drawing ceiling
		Color ceilingColor = ceilTexture->GetColorFromLocation(ceilTexturePos.x, ceilTexturePos.y);
		ceilingColor /= 1.25f; // Dim the color as before
		viewport->AddColorToBuffer(x, height - y - 1, ceilingColor);
	}
}

void WallRaycast(int x, Viewport*& viewport, Player*& player, Camera*& camera, Map*& map, vector<Texture*> textures, bool useASCII)
{
	float& plPosX = player->position.x;
	float& plPosY = player->position.y;

	float& plDirX = player->direction.x;
	float& plDirY = player->direction.y;

	int& width = viewport->size.x;
	int& height = viewport->size.y;

	//  Right of Screen = 1, Left of Screen = - 1
	float cameraX = 2 * x / (float)width - 1.f; // Camera X Position

	// Set the direction of the ray to the players direction + the x axis of the camera
	Vector2 rayDir = Vector2
	(
		plDirX + camera->size.x * cameraX,
		plDirY + camera->size.y * cameraX
	);

	// Which map cell the Player is in
	Vector2i mapPos = Vector2i
	(
		(int)player->position.x,
		(int)player->position.y
	);

	// Distance to next x or y side
	Vector2 sideDist = Vector2();

	// Distance of ray from one x side or y side to next one
	Vector2 deltaDist = Vector2
	(
		// Prevents Dividing by Zero (i.e. very bad) and returns the closest to infinity instead
		// Abs prevents negative distance (impossible)
		(rayDir.x == 0) ? FLT_MAX : std::abs(1.f / rayDir.x),
		(rayDir.y == 0) ? FLT_MAX : std::abs(1.f / rayDir.y)
	);

	// Will be used to calculate total ray length (Perpendicular Wall Distance)
	float perpWallDist;

	// What direction to make in x or y distance
	Vector2i step = Vector2i();

	bool wallHit = false; // If wall was hit
	bool isHorizontalWall; // If a NS or EW wall was hit

	// Calculate step and initial sideDist
	if (rayDir.x < 0)
	{
		step.x = -1;
		sideDist.x = (player->position.x - mapPos.x) * deltaDist.x;
	}
	else
	{
		step.x = 1;
		sideDist.x = (mapPos.x + 1.f - player->position.x) * deltaDist.x;
	}
	if (rayDir.y < 0)
	{
		step.y = -1;
		sideDist.y = (player->position.y - mapPos.y) * deltaDist.y;
	}
	else
	{
		step.y = 1;
		sideDist.y = (mapPos.y + 1.f - player->position.y) * deltaDist.y;
	}

	// DDA algorithm (Digital Differential Analyser)
	while (!wallHit)
	{
		// Go to next map square, either in x or y direction
		if (sideDist.x < sideDist.y)
		{
			sideDist.x += deltaDist.x;
			mapPos.x += step.x;
			isHorizontalWall = false;
		}
		else
		{
			sideDist.y += deltaDist.y;
			mapPos.y += step.y;
			isHorizontalWall = true;
		}

		// Check if ray has hit a wall
		if (map->contents[mapPos.x][mapPos.y] > 0)
		{
			wallHit = true;
		}
	}

	// Calculate distance of wall projected on camera direction (Direct distance to player would cause fisheye effect)
	if (!isHorizontalWall)
	{
		perpWallDist = (sideDist.x - deltaDist.x);
	}
	else
	{
		perpWallDist = (sideDist.y - deltaDist.y);
	}

	// Calculate height of line to draw on screen
	int lineHeight = (int)(height / perpWallDist);

	// Calculate the lowest and highest pixel to fill in current line
	// (Center line in middle of viewport)
	int drawStart = -lineHeight / 2 + height / 2;

	// Prevents drawing off screen
	if (drawStart < 0)
	{
		drawStart = 0;
	}

	int drawEnd = lineHeight / 2 + height / 2;
	if (drawEnd > height)
	{
		// Accounts for Screen starting at 0 and not 1
		drawEnd = height;
	}

	// Checks whether using 24bit color or ASCII Renderer
	if (useASCII)
	{
		// Choose Wall Color
		unsigned char color = GetASCIIColorFromRaycast(mapPos.x, mapPos.y, map, isHorizontalWall);

		// Get Character
		char pixel = viewport->GetCharFromDepth(float((lineHeight >= height) ? height : lineHeight) / float(height));

		//Add Line to Buffer
		viewport->AddScanlineToBuffer(x, height, drawStart, drawEnd, pixel, color, 0);
	}
	else
	{
		// Allows texture 0 to be used
		int texNum = map->contents[mapPos.x][mapPos.y] - 1;

		// Calculate exact part of the wall hit instead of just cell
		float wallX; // Technically its the y cord of the wall if its 
		// horizontal but its the x cord of the texture
		if (isHorizontalWall)
		{
			wallX = plPosX + perpWallDist * rayDir.x;
		}
		else
		{
			wallX = plPosY + perpWallDist * rayDir.y;
		}
		wallX -= floor(wallX); // % of the x coordinate if start = 0 and end = 1

		Vector2i texSize = textures[texNum]->GetSize();

		// Get X Coordinate on the texture

		// Flip Texture Accordingly Depending on if on NSEW Wall
		int texPosX = int(wallX * (float)texSize.x);
		if (isHorizontalWall == 0 && rayDir.x < 0)
		{
			texPosX = texSize.x - texPosX - 1;
		}
		if (isHorizontalWall == 1 && rayDir.y > 0)
		{
			texPosX = texSize.x - texPosX - 1;
		}
		// X coordinate stays the same for each cast but y has to be calculated for each

		// How much to increase the texture coordinate per pixel
		float step = 1.f * texSize.y / lineHeight;

		// Starting texture coordinate
		float texPosY = (drawStart - (height / 2.f) + (lineHeight / 2)) * step;

		// Repeat for each character in the raycast
		Color color;
		for (int y = drawStart; y < drawEnd; y++)
		{

			// Cast the texture coordinate to integer, and bitwise and with (textHeight - 1) for overflow
			int texY = (int)texPosY & (texSize.y - 1);


			color = textures[texNum]->GetColorFromLocation(texPosX, texPosY);
			if (isHorizontalWall)
			{
				color /= 1.5f;
			}
			texPosY += step;

			viewport->AddColorToBuffer(x, y, color);
		}
	}
}

void SortSprites(int* order, float* distance, int amount)
{
}

unsigned char GetASCIIColorFromRaycast(int x, int y, Map*& map, bool isHorizontal)
{
	switch (map->contents[x][y])

	{
	case 1:
	{
		return (isHorizontal) ? CLR_RED : CLR_LIGHTRED;
	}
	case 2:
	{
		return (isHorizontal) ? CLR_GREEN : CLR_LIGHTGREEN;
	}
	case 3:
	{
		return (isHorizontal) ? CLR_CYAN : CLR_LIGHTCYAN;
	}
	case 4:
	{
		return (isHorizontal) ? CLR_MAGENTA : CLR_LIGHTMAGENTA;
	}
	default:
	{
		return (isHorizontal) ? CLR_DARKGREY : CLR_WHITE;
	}
	}
}
