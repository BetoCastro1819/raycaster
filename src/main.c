#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"

const int screenWidth = 800;
const int screenHeigth = 800;
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

	Image screenImage = GenImageColor(screenWidth, screenHeigth, RED);
	Texture2D screenTexture = LoadTextureFromImage(screenImage);

	Player player = {
		.position = (Vector2) { screenWidth/2, screenHeigth/2 },
		.direction = (Vector2) { 0, -100 },
		.rotationSpeed = 0.01f,
		.speed = 200.f
	};

	Vector2 plane = { 100, 0 };

	const int pixelCellSize = screenWidth / mapWidth;

	while (!WindowShouldClose())
	{
/*
		Color* pixels = LoadImageColors(screenImage);
		SetScreenPixelColor(pixels, (int)(screenWidth / 2), (int)(screenHeigth / 2), WHITE);
		UpdateTexture(screenTexture, pixels);
		UnloadImageColors(pixels);
*/

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


		BeginDrawing();

			ClearBackground(WHITE);

			// Draw World Map
			for (int y = 0; y < mapHeight; ++y)
			{
				for (int x = 0; x < mapWidth; ++x)
				{
					DrawLineV((Vector2){ x * pixelCellSize, 0 }, (Vector2){ x * pixelCellSize, screenHeigth }, GRAY);

					if (worldMap[y][x] != 0)
					{
						DrawRectangle(x * pixelCellSize, y * pixelCellSize, pixelCellSize, pixelCellSize, BLUE);
					}
				}
			
				DrawLineV((Vector2){ 0, y * pixelCellSize }, (Vector2){ screenWidth, y * pixelCellSize }, GRAY);
			}

			// Draw Player
			DrawCircleV(player.position, 10.0f, GREEN);
			DrawLineV(player.position, Vector2Add(player.position, player.direction), BLACK);
			
			Vector2 planePos = Vector2Add(player.position, player.direction);
			Vector2 planeLeftEdge = Vector2Subtract(planePos, plane);
			DrawLineV(planePos, planeLeftEdge, BLUE);
			Vector2 planeRightEdge = Vector2Add(planePos, plane);
			DrawLineV(planePos, planeRightEdge, BLUE);


			Vector2 playerMapPosition = { player.position.x / (float)pixelCellSize, player.position.y / (float)pixelCellSize };

			for (int i = 0; i < screenWidth; ++i)
			{
				if (i != (screenWidth / 2)) continue;

				// X-coordinate in camera space
				float cameraX = 2.f * (float)i / (float)screenWidth - 1.f;
				Vector2 rayDir = Vector2Add(player.direction, Vector2Scale(plane, cameraX));
				rayDir = Vector2Normalize(rayDir);
				DrawLineV(player.position, Vector2Add(player.position, Vector2Scale(rayDir, 1000)), RED);

				double deltaDistX = AlmostZero(rayDir.x) ? 1e30 : fabs(1.f / rayDir.x);
				double deltaDistY = AlmostZero(rayDir.y) ? 1e30 : fabs(1.f / rayDir.y);

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

				SideHitType sideHitType;
				bool hit = false;
				int segmentCount = 0;
				while (!hit && segmentCount < mapWidth)
				{
					Vector2 pixelSideDist = { sideDistX * (float)pixelCellSize, sideDistY * (float)pixelCellSize };
					// printf("sideDist: { %f, %f }\n", sideDistX, sideDistY);
					// printf("pixelSideDist: { %i, %i }\n", (int)pixelSideDist.x, (int)pixelSideDist.y);
					DrawCircleV(Vector2Add(player.position, Vector2Scale(rayDir, pixelSideDist.x)), 3.f, RED);
					DrawCircleV(Vector2Add(player.position, Vector2Scale(rayDir, pixelSideDist.y)), 3.f, GREEN);

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
			}

			//DrawTexture(screenTexture, 0, 0, RAYWHITE);
		
		EndDrawing();
	}

	UnloadTexture(screenTexture);
	UnloadImage(screenImage);

	CloseWindow();
	return 0;
}