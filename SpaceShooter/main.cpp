#include <raylib.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// BEFORE READING
// DISCLAIMER:
/*
    This Project was in no way possible made with modular optimized code, its simply a playground for me
    to try different RayLib aspects, Hope you have fun lol
*/

struct Bullet {
    Vector2 Position;
    Vector2 Velocity;
    Rectangle bounds;
    bool alive;
};

struct Enemy {
    Vector2 Position;
    Vector2 Velocity;
    float speed;
    Rectangle bounds;
    Texture2D type;
    bool alive;
};

const int STARS = 200;
typedef struct {
    float x,    //The stars coordinates
        y,    //on the screen
        z;    //The stars depth or distance from camera
}Star;

// Random Function for Stars Paralaxx Background 
float randf() {
    return (rand() % 1000) / 1000.0f;
}

// Function to draw the health bar for the player
void DrawHealthBar(int x, int y, int width, int height, float health) {
    int barWidth = (int)((health / 100.0f) * width);
    Color healthColor = (health > 50) ? GREEN : RED;
    DrawRectangle(x, y, width, height, GRAY);
    DrawRectangle(x, y, barWidth, height, healthColor);
}

int main() {
    srand(time(NULL));
    const int screenWidth = 1024;
    const int screenHeight = 1024;
    const float shipSpeed = 400.0f;
    const float bulletSpeed = 1400.0f;
    const float enemySpeed = 150.0f;
    int playerHealth = 400;
    const float healthDecrementRate = 1.0f; // 1 point per second
    bool gameOver = false;
    bool InGame = false;
    // Music for the main game
    Music MainMusic = LoadMusicStream("C:/Users/PC/Downloads/music.mp3");
    

    // Main menu timers
    float waitTime = 0.5f;

    SetTargetFPS(60);

    
    InitWindow(screenWidth, screenHeight, "Space Shooter");
    InitAudioDevice();

    Vector2 shipPosition = { GetScreenWidth() / 2, GetScreenHeight() / 2 };
    Vector2 shipVelocity = { 0, 0 };

    // Load textures / Sounds
    Texture2D shipSprite = LoadTexture("C:/Users/PC/Downloads/Compressed/assets/ship.png");
    Texture2D bulletSprite = LoadTexture("C:/Users/PC/Downloads/laser.png");
    Texture2D GameBackground = LoadTexture("C:/Users/PC/Downloads/background.gif");
    Sound gunSound = LoadSound("C:/Users/PC/Downloads/Music/laserGun.mp3");
    Texture2D explosionSprite = LoadTexture("C:/Users/PC/Downloads/Compressed/explosion.png"); // Explosion sprite
    Rectangle sourceRect = { 0, 0, 80, 80 };
    Rectangle ShipBoundaries = { shipPosition.x, shipPosition.y, (float)shipSprite.width, (float)shipSprite.height };
    int currentFrame = 1;
    int score = 0;
    const char* gameOverText = "GAME OVER";
    /// ****** ENEMY TODO ****** ///
    // - [] Add Textures for all enemies
    // - [] Add Boss Functionality
    // - [] Create a List of all textures , randomly pick one of them, adjust speed depending on type 



    const int healthBarWidth = 200;
    const int healthBarHeight = 20;
    const int healthBarX = (screenWidth - healthBarWidth) / 2;
    const int healthBarY = screenHeight - 50;
    int health = 100;

    // Spawn Variables
    double spawnCooldown = 0.0;
    double spawnInterval = 1.5f;

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    // Main Menu Variables
    Texture2D MenuBackground = LoadTexture("C:/Users/PC/Downloads/background.png");
    Texture2D Title = LoadTexture("C:/Users/PC/Downloads/title.png");
    bool InMainMenu = true;

    // Handle Enemy Sprites 
    Texture2D GreenEnemy = LoadTexture("C:/Users/PC/Downloads/green_enemy.png");
    Rectangle GreenEnemySR = { 0,0, 64, 64 }; // SR -> Source Rectangle 
    int currentEnemyFrame = 0;
    float elapsedTime = 0.0f;
    const float frameDelay = 0.5f; // Delay so the frames don't switch too quickly

    // Stars Effect for background
    
    Star stars[STARS] = { 0 }; // Make 200 Stars

    // ****** RANDOMIZE STAR POSITIONS ****** //
    for (int i = 0; i < STARS; i++) {
        stars[i].x = GetRandomValue(0, screenWidth);
        stars[i].y = GetRandomValue(0, screenHeight);
        stars[i].z = randf();
    }



    PlayMusicStream(MainMusic);
    // Main game loop
    while (!WindowShouldClose()) {
        elapsedTime += GetFrameTime();

        SetSoundVolume(gunSound, 0.3f);
        SetMusicVolume(MainMusic, 0.6f);
        float FrameTime = GetFrameTime();
        spawnCooldown += GetFrameTime();

        if (InMainMenu) {



            playerHealth = 400;
            score = 0;
            bullets.clear();
            enemies.clear();

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(MenuBackground, 0, 0, WHITE);
            DrawTexture(Title, (screenWidth - Title.width) / 2, (screenHeight / 2) - 400, WHITE);
            // Start Game text Variables
            if (elapsedTime >= waitTime) {
                    int textWidth = MeasureText("PRESS SPACE TO START!", 50);
                    int textX = (GetScreenWidth() - textWidth) / 2;
                    int textY = (GetScreenHeight() / 2) + 300; // Adjust the Y position as needed
                    DrawText("PRESS SPACE TO START!", textX, textY, 50, WHITE);
            }

            if (elapsedTime >= 2 * waitTime) {
                elapsedTime = 0.0f;
            }

            if (IsKeyPressed(KEY_SPACE)) { // Detect Button Hover
                InGame = true;
                InMainMenu = false;
            }

                
            EndDrawing();
            continue;
        }

        if (gameOver) {
            BeginDrawing();
            ClearBackground(BLACK);
            int gameOverTextWidth = MeasureText(gameOverText, 30);
            int pressEnterTextWidth = MeasureText("Press ENTER to play again", 30);
            int xPos = (screenWidth - gameOverTextWidth) / 2;
            int yPos = screenHeight / 2 - 30 / 2;

            // reset the game 
            playerHealth = 400;
            score = 0;
            bullets.clear();
            enemies.clear();

            DrawText(gameOverText, xPos, yPos, 30, WHITE);
            DrawText("Press ENTER to play again", (screenWidth - pressEnterTextWidth) / 2, yPos + 40, 30, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                InGame = true;
            }
            EndDrawing();
        }

        // Handle Spawn Cooldown for enemies
        if (spawnCooldown >= spawnInterval) {
            Enemy enemy;
            enemy.Position = { (float)GetRandomValue(0 + GreenEnemy.width + (GreenEnemy.width / 2), GetScreenWidth()) - GreenEnemy.width, -10 };
            enemy.Velocity.y = enemySpeed;
            enemy.bounds = { enemy.Position.x, enemy.Position.y, (float)GreenEnemy.width, (float)GreenEnemy.height };
            enemy.alive = true;
            enemies.push_back(enemy);
            spawnCooldown = 0.0;
        }

        // Key input for each keystroke
        if (IsKeyDown(KEY_W)) shipVelocity.y = -shipSpeed;
        else if (IsKeyDown(KEY_S)) shipVelocity.y = shipSpeed;
        else shipVelocity.y = 0;

        if (IsKeyDown(KEY_D)) {
            shipVelocity.x = shipSpeed;
            currentFrame = 2;
        }
        else if (IsKeyDown(KEY_A)) {
            shipVelocity.x = -shipSpeed;
            currentFrame = 0;
        }
        else {
            shipVelocity.x = 0;
            currentFrame = 1;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            Bullet bullet;
            bullet.Position = { shipPosition.x + sourceRect.width / 2, shipPosition.y + sourceRect.height / 2 - 10 };
            bullet.Velocity.y = -bulletSpeed;
            bullet.bounds = { bullet.Position.x, bullet.Position.y, (float)bulletSprite.width, (float)bulletSprite.height };
            bullet.alive = true;
            bullets.push_back(bullet);
            PlaySound(gunSound);
        }

        // Update Location of enemy and bullet
        for (auto& enemy : enemies) {
            enemy.Position.y += enemy.Velocity.y * FrameTime;
            if (enemy.Position.y > GetScreenHeight()) {
                enemy.alive = false;
            }
        }

        for (auto& bullet : bullets) {
            bullet.Position.y += bullet.Velocity.y * FrameTime;
            if (bullet.Position.y < 0) {
                bullet.alive = false;
            }
        }

        // Update Ship Location
        shipPosition.y += shipVelocity.y * FrameTime;
        shipPosition.x += shipVelocity.x * FrameTime;

        // Update the boundaries with the location of the ship
        ShipBoundaries.x = shipPosition.x;
        ShipBoundaries.y = shipPosition.y;

        if (shipPosition.x < 0) shipPosition.x = 0;
        if (shipPosition.y < 0) shipPosition.y = 0;
        if (shipPosition.x + sourceRect.width > screenWidth) shipPosition.x = screenWidth - sourceRect.width;
        if (shipPosition.y + sourceRect.height > screenHeight) shipPosition.y = screenHeight - sourceRect.height;

        // Clamp Health Range
        if (playerHealth < 0) playerHealth = 0;
        if (playerHealth > 100) playerHealth = 100;

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) { return !bullet.alive; }), bullets.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) { return !enemy.alive; }), enemies.end());

        // Handle Enemy Frames
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime;
        if (elapsedTime >= frameDelay) {
            currentEnemyFrame = (currentEnemyFrame + 1) % 3;
            elapsedTime = 0.0f;
        }

        // Drawing
        if (InGame) {
            if (!gameOver) {
                //-----SCROLL THE STARS-----//
                for (int i = 0; i < STARS; i++) {
                    stars[i].y += 10.0f * (stars[i].z / 1); // Speed Depending on Z index , Giving the illusion of far stars moving slower

                    if (stars[i].y >= screenHeight) {
                        stars[i].y -= screenHeight;
                        stars[i].x = GetRandomValue(0, screenWidth);
                    }
                }


                BeginDrawing();

                ClearBackground(BLACK);

                for (int i = 0; i < STARS; i++) {
                    float x = stars[i].x;
                    float y = stars[i].y;

                    DrawPixel(x, y, WHITE);
                }





                DrawText(TextFormat("SCORE : %i", score), 10, 10, 40, YELLOW);
                DrawHealthBar(healthBarX, healthBarY, healthBarWidth, healthBarHeight, playerHealth);
                sourceRect.x = currentFrame * (shipSprite.width / 3);
                GreenEnemySR.x = currentEnemyFrame * (GreenEnemy.width / 3);
                for (auto& bullet : bullets) DrawTexture(bulletSprite, bullet.Position.x, bullet.Position.y, WHITE);
                for (auto& enemy : enemies) DrawTextureRec(GreenEnemy, GreenEnemySR, enemy.Position, WHITE);

                // Draw explosion at enemy position if alive
                for (auto& bullet : bullets) {
                    bullet.bounds = { bullet.Position.x, bullet.Position.y, (float)bulletSprite.width, (float)bulletSprite.height };
                    for (auto& enemy : enemies) {
                        enemy.bounds = { enemy.Position.x, enemy.Position.y, (float)GreenEnemy.width, (float)GreenEnemy.height };
                        if (CheckCollisionRecs(bullet.bounds, enemy.bounds)) {
                            enemy.alive = false;
                            bullet.alive = false;
                            score++; // Increment Score when Enemy Dies
                        }
                    }
                }

                // Handle ship collision with enemies
                float healthDecrement = healthDecrementRate * GetFrameTime();
                bool shipCollidedThisFrame = false; // make it so the ship collides each one frame only
                for (auto& enemy : enemies) {
                    enemy.bounds = { enemy.Position.x, enemy.Position.y, (float)GreenEnemy.width, (float)GreenEnemy.height };
                    if (!shipCollidedThisFrame && CheckCollisionRecs(enemy.bounds, ShipBoundaries)) {
                        std::cout << playerHealth << std::endl;
                        playerHealth -= healthDecrement;
                        shipCollidedThisFrame = true;
                        if (playerHealth <= 0) {
                            gameOver = true;
                        }
                    }
                }

                DrawTextureRec(shipSprite, sourceRect, shipPosition, WHITE);
                EndDrawing();
            }
        }
    }

    // Free the Memory
    UnloadTexture(shipSprite);
    UnloadTexture(bulletSprite);
    UnloadTexture(MenuBackground);
    UnloadTexture(GreenEnemy);
    UnloadTexture(explosionSprite); // Free explosion sprite
    UnloadTexture(Title);
    UnloadMusicStream(MainMusic);
    UnloadSound(gunSound);
    CloseWindow();
    CloseAudioDevice();
    return 0;
}
