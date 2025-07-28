#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"

const int screenWidth = 900;
const int screenHeigth = 600;
#define mapWidth 24
#define mapHeight 24

typedef struct Player {
	Vector2 position;
	Vector2 direction;
	float rotationSpeed;
	float speed;
} Player;

typedef enum SideHitType {
	VERTICAL,
	HORIZONTAL
} SideHitType;

bool AlmostZero(float val)
{
	return (val > 0.0f && val < 0.000001f);
}

void SetScreenPixelColor(Color* pixelsArray, int posX, int posY, Color pixelColor)
{
	pixelsArray[posX + posY * screenWidth] = pixelColor;
}

Color GetWallColor(int wallType)
{
	Color wallColor;
	switch (wallType)
	{
	case 1: wallColor = RED; break;
	case 2: wallColor = GREEN; break;
	case 3: wallColor = BLUE; break;
	case 4: wallColor = GRAY; break;
	default: wallColor = MAGENTA; break;
	}

	return wallColor;
}

int worldMap[mapWidth][mapHeight] =
{
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1 },
	{ 1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1 },
	{ 1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
	{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }
};

int main()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	
	InitWindow(screenWidth, screenHeigth, "Simple Raycaster");

	Player player = {
		.position = (Vector2) { screenWidth/2, screenHeigth/2 },
		.direction = (Vector2) { 0, -100 },
		.rotationSpeed = 0.1f,
		.speed = 150.f
	};

	Vector2 plane = { 80, 0 };

	const int pixelCellSize = screenWidth / mapWidth;

	Vector2 screenCenter = { screenWidth/2, screenHeigth/2 };
	bool draw2D = false;

	Image screenImage = GenImageColor(screenWidth, screenHeigth, WHITE);
	Texture screenTexture = LoadTextureFromImage(screenImage);

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		Vector2 normalizedDir = Vector2Normalize(player.direction);

		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		{
			player.position = Vector2Add(player.position, Vector2Scale(normalizedDir, player.speed * deltaTime));
		}
		if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		{
			player.position = Vector2Subtract(player.position, Vector2Scale(normalizedDir, player.speed * deltaTime));
		}
		if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
		{
			player.direction = Vector2Rotate(player.direction, player.rotationSpeed);
			plane = Vector2Rotate(plane, player.rotationSpeed);
		}
		if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
		{
			player.direction = Vector2Rotate(player.direction, -player.rotationSpeed);
			plane = Vector2Rotate(plane, -player.rotationSpeed);
		}

		if (IsKeyPressed(KEY_M))
		{
			draw2D = !draw2D;
		}

		BeginDrawing();

			ClearBackground(WHITE);

			// DrawRectangle(screenCenter.x - 10, screenCenter.y - 10, 20, 20, RED);

			Color* pixels = LoadImageColors(screenImage);

			if (draw2D)
			{
				// Draw World Map
				for (int y = 0; y < mapHeight; ++y)
				{
					for (int x = 0; x < mapWidth; ++x)
					{
						// DrawLineV((Vector2){ relativePosX, relativePosY }, (Vector2){ relativePosX, screenHeigth - relativePosY }, GRAY);

						if (worldMap[y][x] != 0)
						{
							DrawRectangle(
								screenCenter.x + x * pixelCellSize - player.position.x,
								screenCenter.y + y * pixelCellSize - player.position.y,
								pixelCellSize, pixelCellSize, GetWallColor(worldMap[y][x]));
						}
					}
				
					// DrawLineV((Vector2){ player.position.x - screenWidth, player.position.y + y * pixelCellSize }, (Vector2){ player.position.x - screenWidth, player.position.y + y * pixelCellSize }, GRAY);
				}

				// Draw Player
				DrawCircleV(screenCenter, 10.0f, GREEN);
				DrawLineV(screenCenter, Vector2Add(screenCenter, player.direction), BLACK);
				
				// Draw view plane
				Vector2 planePos = Vector2Add(screenCenter, player.direction);
				Vector2 planeLeftEdge = Vector2Subtract(planePos, plane);
				DrawLineV(planePos, planeLeftEdge, BLUE);
				Vector2 planeRightEdge = Vector2Add(planePos, plane);
				DrawLineV(planePos, planeRightEdge, BLUE);
			}

			Vector2 playerMapPosition = { player.position.x / (float)pixelCellSize, player.position.y / (float)pixelCellSize };

			for (int i = 0; i < screenWidth; ++i)
			{
				// X-coordinate in camera space
				float cameraX = 2 * i / (float)screenWidth - 1;
				Vector2 rayDir = Vector2Add(player.direction, Vector2Scale(plane, cameraX));
				rayDir = Vector2Normalize(rayDir);
				// DrawLineV(player.position, Vector2Add(player.position, Vector2Scale(rayDir, 1000)), RED);

				double deltaDistX = AlmostZero(rayDir.x) ? 1e30 : fabs(1 / rayDir.x);
				double deltaDistY = AlmostZero(rayDir.y) ? 1e30 : fabs(1 / rayDir.y);

				Vector2 rayCuadrantPosition = { (int)playerMapPosition.x, (int)playerMapPosition.y };

				// Direction to step in (can be +1 or -1)
				Vector2 stepDir;

				// Length of ray from current position to the nex X or Y side
				double sideDistX;
				double sideDistY;

				if (rayDir.x < 0.f)
				{
					stepDir.x = -1;
					sideDistX = (playerMapPosition.x - rayCuadrantPosition.x) * deltaDistX;
				}
				else
				{
					stepDir.x = 1;
					sideDistX = (rayCuadrantPosition.x + 1.0f - playerMapPosition.x) * deltaDistX;
				}

				if (rayDir.y < 0.f)
				{
					stepDir.y = -1;
					sideDistY = (playerMapPosition.y - rayCuadrantPosition.y) * deltaDistY;
				}
				else
				{
					stepDir.y = 1;
					sideDistY = (rayCuadrantPosition.y + 1.0f - playerMapPosition.y) * deltaDistY;
				}

				double perpWallDist;
				SideHitType sideHitType;
				bool hit = false;
				int segmentCount = 0;
				while (!hit) // && segmentCount <= mapWidth)
				{
					// Vector2 pixelSideDist = { sideDistX * (float)pixelCellSize, sideDistY * (float)pixelCellSize };
					// printf("sideDist: { %f, %f }\n", sideDistX, sideDistY);
					// printf("pixelSideDist: { %i, %i }\n", (int)pixelSideDist.x, (int)pixelSideDist.y);
					// DrawCircleV(Vector2Add(player.position, Vector2Scale(rayDir, pixelSideDist.x)), 3.f, RED);
					// DrawCircleV(Vector2Add(player.position, Vector2Scale(rayDir, pixelSideDist.y)), 3.f, GREEN);

					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						rayCuadrantPosition.x += stepDir.x;
						sideHitType = VERTICAL;
					}
					else
					{
						sideDistY += deltaDistY;
						rayCuadrantPosition.y += stepDir.y;
						sideHitType = HORIZONTAL;
					}

					if (worldMap[(int)rayCuadrantPosition.y][(int)rayCuadrantPosition.x] != 0)
					{
						hit = true;
					}

					segmentCount++;
				}

				if (!draw2D)
				{
					if (sideHitType == VERTICAL)
						perpWallDist = Vector2Length(Vector2Scale(rayDir, (sideDistX - deltaDistX))) * cosf(Vector2Angle(player.direction, rayDir));
					else
						perpWallDist = Vector2Length(Vector2Scale(rayDir, (sideDistY - deltaDistY))) * cosf(Vector2Angle(player.direction, rayDir));

					int lineHeight = (int)(1 * screenHeigth / perpWallDist);

					int drawStart = -lineHeight / 2 + screenHeigth / 2;
					if (drawStart < 0)
						drawStart = 0;

					int drawEnd = lineHeight / 2 + screenHeigth / 2;
					if (drawEnd >= screenHeigth)
						drawEnd = screenHeigth - 1;

					Color wallColor = GetWallColor(worldMap[(int)rayCuadrantPosition.y][(int)rayCuadrantPosition.x]);

					if (sideHitType == HORIZONTAL)
						wallColor = ColorBrightness(wallColor, -0.1f);

					for (int posY = drawStart; posY < drawEnd; posY++)
					{
						SetScreenPixelColor(pixels, i, posY, wallColor);
					}
				}
			}

			int x = screenWidth/2;
			int y = screenHeigth/2;
			pixels[x + y * screenWidth] = RED;

			UpdateTexture(screenTexture, pixels);
            UnloadImageColors(pixels);
			DrawTexture(screenTexture, 0, 0, WHITE);

			// Draw UI
			DrawText(TextFormat("Player position: %.2f %.2f", player.position.x, player.position.y), 10, 10, 20, BLACK);

		EndDrawing();
	}

	UnloadTexture(screenTexture);
	UnloadImage(screenImage);

	CloseWindow();
	return 0;
}