#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"

const int screenWidth = 900;
const int screenHeight = 600;
#define mapWidth 24
#define mapHeight 24
#define wallTextureCount 8
#define wallTextureSize 64

typedef struct Player {
	Vector2 position;
	Vector2 velocity;
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
	{ 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7 },
	{ 4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7 },
	{ 4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7 },
	{ 4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7 },
	{ 4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7 },
	{ 4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7 },
	{ 4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1 },
	{ 4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8 },
	{ 4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1 },
	{ 4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8 },
	{ 4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1 },
	{ 4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1 },
	{ 6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6 },
	{ 8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4 },
	{ 6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6 },
	{ 4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3 },
	{ 4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2 },
	{ 4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2 },
	{ 4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2 },
	{ 4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2 },
	{ 4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2 },
	{ 4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2 },
	{ 4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2 },
	{ 4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3 }
};

int main()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	
	InitWindow(screenWidth, screenHeight, "Simple Raycaster");

	Player player = {
		.position = (Vector2) { screenWidth/2, screenHeight/2 },
		.velocity = (Vector2) { 0, 0 },
		.direction = (Vector2) { 0, -100 },
		.rotationSpeed = 0.05f,
		.speed = 150.f
	};

	Vector2 plane = { 80, 0 };

	const int pixelCellSize = screenWidth / mapWidth;

	Vector2 screenCenter = { screenWidth/2, screenHeight/2 };
	bool draw2D = false;

	Image screenImage = GenImageColor(screenWidth, screenHeight, WHITE);
	Texture screenTexture = LoadTextureFromImage(screenImage);

	// 512 x 64
	Image wallImages = LoadImage("resources/wall_textures.png");
	Color* wallImagesPixels = LoadImageColors(wallImages);

	double debugFloat = 0.0;
	double debugFloat2 = 0.0;

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		Vector2 normalizedDir = Vector2Normalize(player.direction);

		// TODO: fix acceleration physics
		float acceleration = 10.0f;
		float maxVelocity = 2.0f;
		float maxVelocitySqr = maxVelocity * maxVelocity;

		bool isMoving = false;

		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
		{
			if (Vector2LengthSqr(player.velocity) < maxVelocitySqr)
			{
				player.velocity = Vector2Add(player.velocity, Vector2Scale(normalizedDir, acceleration * deltaTime));
			}
			else
			{
				player.velocity = Vector2Add(player.velocity, Vector2Scale(normalizedDir, maxVelocity));
			}

			isMoving = true;
			// player.position = Vector2Add(player.position, Vector2Scale(normalizedDir, player.speed * deltaTime));
		}
		if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
		{
			if (Vector2LengthSqr(player.velocity) < maxVelocitySqr)
			{
				player.velocity = Vector2Subtract(player.velocity, Vector2Scale(normalizedDir, acceleration * deltaTime));
			}
			else
			{
				player.velocity = Vector2Subtract(player.velocity, Vector2Scale(normalizedDir, maxVelocity));
			}

			isMoving = true;
			// player.position = Vector2Subtract(player.position, Vector2Scale(normalizedDir, player.speed * deltaTime));
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

		float velocityLength = Vector2Length(player.velocity);
		if (!isMoving)
		{
			if (velocityLength < 0.1f || Vector2DotProduct(normalizedDir, player.velocity) < 0.0f)
			{
				player.velocity = (Vector2) { 0, 0 };
			}
			else
			{
				player.velocity = Vector2Subtract(player.velocity, Vector2Scale(normalizedDir, (acceleration/2) * deltaTime));
			}
		}

		player.position = Vector2Add(player.position, player.velocity);

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
				while (!hit && segmentCount <= mapWidth + 5)
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

				double scaleFactor = (sideHitType == VERTICAL) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
				Vector2 rayHit = Vector2Scale(rayDir, scaleFactor);
				perpWallDist = Vector2Length(rayHit) * cos(Vector2Angle(player.direction, rayDir));

				double wallCoordX;
				if (sideHitType == VERTICAL)
				{
					wallCoordX = playerMapPosition.y + perpWallDist * rayDir.y;
				}
				else
				{
					wallCoordX = playerMapPosition.x + perpWallDist * rayDir.x;
				}
				wallCoordX -= floor(wallCoordX);

				int textureX = (int)(wallCoordX * (double)wallTextureSize);
				if (sideHitType == VERTICAL && rayDir.x > 0 ||
					sideHitType == HORIZONTAL && rayDir.y < 0)
				{
					textureX = wallTextureSize - textureX - 1;
				}

				if (i == screenWidth/2)
				{
					// debugFloat = wallCoordX;
					// debugFloat2 = textureX;

					DrawCircleV(Vector2Add(screenCenter, Vector2Scale(rayDir, Vector2Length(rayHit) * pixelCellSize)) , 3.f, RED);
				}

				Color wallColor = GetWallColor(worldMap[(int)rayCuadrantPosition.y][(int)rayCuadrantPosition.x]);

				if (sideHitType == HORIZONTAL)
					wallColor = ColorBrightness(wallColor, -0.1f);

				if (!draw2D)
				{
					int lineHeight = (int)(1 * screenHeight / perpWallDist);

					int drawStart = -lineHeight / 2 + screenHeight / 2;
					if (drawStart < 0)
						drawStart = 0;

					int drawEnd = lineHeight / 2 + screenHeight / 2;
					if (drawEnd >= screenHeight)
						drawEnd = screenHeight - 1;

					int wallTextureIndex = -1;
					if (rayCuadrantPosition.y >= 0 && rayCuadrantPosition.y < mapHeight &&
						rayCuadrantPosition.x >= 0 && rayCuadrantPosition.x < mapWidth)
					{
						wallTextureIndex = worldMap[(int)rayCuadrantPosition.y][(int)rayCuadrantPosition.x];
					}

/*
						// How much to increase the texture coordinate per screen pixel
						double step = 1.0 * texHeight / lineHeight;
						// Starting texture coordinate
						double texPos = (drawStart - h / 2 + lineHeight / 2) * step;
						for(int y = drawStart; y<drawEnd; y++)
						{
							// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
							int texY = (int)texPos & (texHeight - 1);
							texPos += step;
							Uint32 color = texture[texNum][texHeight * texY + texX];
							//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
							if(side == 1) color = (color >> 1) & 8355711;
							buffer[y][x] = color;
						}
*/

					// How much to increase the texture coordinate per screen pixel
					double step = (double)wallTextureSize / lineHeight;
					// Starting texture coordinate
					double texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;

					for (int posY = drawStart; posY < drawEnd; posY++)
					{
						// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
						int textureY = (int)texPos & (wallTextureSize - 1);
						texPos += step;

						Color texturePixelColor = MAGENTA;
						if (wallTextureIndex > -1)
						{
							texturePixelColor = wallImagesPixels[wallTextureIndex * wallTextureSize + textureX + textureY * wallTextureSize * wallTextureCount];
						}

						if (sideHitType == VERTICAL)
						{
							texturePixelColor.r = (unsigned char)((float)texturePixelColor.r * 0.5);
							texturePixelColor.g = (unsigned char)((float)texturePixelColor.g * 0.5);
							texturePixelColor.b = (unsigned char)((float)texturePixelColor.b * 0.5);
						}

						SetScreenPixelColor(pixels, i, posY, texturePixelColor);
					}
				}
			}

			UpdateTexture(screenTexture, pixels);
			UnloadImageColors(pixels);

			if (!draw2D)
			{
				DrawTexture(screenTexture, 0, 0, WHITE);
			}

			// Draw UI
			// DrawText(TextFormat("Player position: %.2f %.2f", player.position.x, player.position.y), 10, 10, 20, BLACK);
			DrawText(TextFormat("PerpWallDist: %.2f - Player dir-ray angle: %.2f", debugFloat, debugFloat2), 10, 10, 20, BLACK);

		EndDrawing();
	}

	UnloadTexture(screenTexture);
	UnloadImage(screenImage);

	UnloadImage(wallImages);

	CloseWindow();
	return 0;
}