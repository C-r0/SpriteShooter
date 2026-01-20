#include <raylib.h>
#include <unistd.h>
#include <cmath>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>

// Game State //
enum GameState {
    SPLASH,
    MENU,
    OPTIONS,
    GAME1,
    GAME2,
    GAME3
};

struct Bullet {
    Vector2 position;
    Vector2 speed;
    bool active;
    int currentFrame;
    float frameTime;
    float angle;
};

struct Zombie {
    Vector2 position;
    float speed;
    float angle;
};

Rectangle spawnzombies1 = {-50, 50, 50, 50};
Rectangle spawnzombies2 = {1350, 50, 50, 50};

void spawnzombies(std::vector<Zombie>& zombies, int maxZombies)
{
    while (zombies.size() < maxZombies)
    {
        Zombie z;
        int spawnnumber = GetRandomValue(1, 2);

        if (spawnnumber == 1)
            z.position = { spawnzombies1.x, spawnzombies1.y };
        else
            z.position = { spawnzombies2.x, spawnzombies2.y };

        z.speed = 100.0f;
        z.angle = 0.0f;

        zombies.push_back(z);
    }
}



int main() {
    float master_volume = 0.5f;

    // Setting Window //
    InitWindow(1280, 720, "SpriteShooter");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetMasterVolume(master_volume);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    Image icon = LoadImage("assets/sprites/icon.png");
    Sound splash = LoadSound("assets/audios/splash.mp3");
    Sound shot = LoadSound("assets/audios/shot.mp3");
    Music monument = LoadMusicStream("assets/audios/monument.mp3");
    monument.looping = true;
    SetWindowIcon(icon);

    bool debugMode = false;

    // Configurações do tiro
    Texture2D bulletTexture = LoadTexture("assets/sprites/tiro1-sheet.png");
    int bulletFrames = 3; // 3 frames horizontais
    int bulletFrameWidth = bulletTexture.width / bulletFrames;
    int bulletFrameHeight = bulletTexture.height;

    std::vector<Bullet> bullets;

    SetTargetFPS(60);

    sleep(2.5); // Sleep waiting for splash //
    PlaySound(splash);

    // SPLASH Variables //
    GameState currentState = SPLASH;

    float alpha = 1.0f;
    float fadeSpeed = 0.15f;

    Texture2D mouse = LoadTexture("assets/sprites/cursor.png");
    

    // Games Variables //
    bool game1 = false;
    bool game2 = false;
    bool musicStarted = false;

    // MENU Variables //
    float menuAlpha = 0.0f;
    float menuFadeSpeed = 0.5f;
    Texture2D backgroundmenu = LoadTexture("assets/sprites/background/backgroundmenu.png");

    // GAME1 Variables //
    Texture2D gun1 = LoadTexture("assets/sprites/gun1.png");
    Texture2D backgroundgame1 = LoadTexture("assets/sprites/background/backgroundgame1.png");
    Texture2D player = LoadTexture("assets/sprites/player.png");
    Texture2D playergun1 = LoadTexture("assets/sprites/playerwithgun1.png");
    bool gunPicked = false;
    bool hasGun = false;
    bool dooropen = false;
    bool enemiesdefeated = false;

    // GAME2 Variables
    Texture2D backgroundgame2 = LoadTexture("assets/sprites/background/backgroundgame2.png");
    Texture2D dooriron = LoadTexture("assets/sprites/dooriron.png");
    Texture2D zombie1 = LoadTexture("assets/sprites/zombie1.png");
    bool teleport1done = false;
    std::vector<Zombie> zombies;

    // Setting Objects //
    Rectangle rgun1 = {((float)GetScreenWidth() / 2) - 70, (float)GetScreenHeight() / 2, (float)gun1.width * 2.0f, (float)gun1.height * 2.0f};
    Rectangle rplayer = {((float)GetScreenWidth() / 2) - 30, ((float)GetScreenHeight() / 2) + 300, (float)player.width * 5.0f, (float)player.height * 5.0f};
    Rectangle rdoor1 = {574, 33, 110, 80};
    Rectangle rdoor2 = {567, 4, 120, 120};

    while (!WindowShouldClose()) {
		
		if (currentState == MENU && !musicStarted) {
    PlayMusicStream(monument);
    musicStarted = true;
}
UpdateMusicStream(monument);


		
        float dt = GetFrameTime();
        Vector2 mousePos = GetMousePosition();

        HideCursor();

        BeginDrawing();
        ClearBackground(BLACK);

        // Mouse
        DrawTextureEx(mouse, Vector2{mousePos.x - 8, mousePos.y - 8}, 0.0f, 2.0f, WHITE);

        // Fade SPLASH
        if (currentState == SPLASH) {
            alpha -= fadeSpeed * dt;
            if (alpha <= 0.0f) {
                alpha = 0.0f;
                currentState = MENU;
            }

            const char* text = "SpriteShooter";
            int fontSize = 40;
            int textWidth = MeasureText(text, fontSize);

            DrawText(text, (GetScreenWidth() - textWidth) / 2, GetScreenHeight() / 2, fontSize, Fade(WHITE, alpha));
        }

        // MENU
        else if (currentState == MENU) {
            if (menuAlpha < 1.0f) {
                menuAlpha += menuFadeSpeed * dt;
                if (menuAlpha > 1.0f) menuAlpha = 1.0f;
            }

            // Menu Background
            DrawTexturePro(backgroundmenu, Rectangle{0,0,(float)backgroundmenu.width,(float)backgroundmenu.height},
                            Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()}, Vector2{0,0}, 0.0f, Fade(WHITE, menuAlpha));

            // Botão Play
            int fontSize = 35;
            int textWidth = MeasureText("Play", fontSize);
            float buttonPlayX = ((GetScreenWidth() - textWidth) / 2);
            float buttonPlayY = (GetScreenHeight() / 2);
            Rectangle playButton = {buttonPlayX, buttonPlayY, (float)textWidth, (float)fontSize};
            if (CheckCollisionPointRec(mousePos, playButton)) {
                DrawText("Play", buttonPlayX, buttonPlayY, fontSize, Fade(YELLOW, menuAlpha));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = GAME1;
            } else DrawText("Play", buttonPlayX, buttonPlayY, fontSize, Fade(WHITE, menuAlpha));

            // Botão Options
            int textWidth2 = MeasureText("Options", fontSize);
            float buttonOptionsX = ((GetScreenWidth() - textWidth2) / 2);
            float buttonOptionsY = (GetScreenHeight() / 2) + 60;
            Rectangle optionsButton = {buttonOptionsX, buttonOptionsY, (float)textWidth2, (float)fontSize};
            if (CheckCollisionPointRec(mousePos, optionsButton)) {
                DrawText("Options", buttonOptionsX, buttonOptionsY, fontSize, Fade(YELLOW, menuAlpha));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = OPTIONS;
            } else DrawText("Options", buttonOptionsX, buttonOptionsY, fontSize, Fade(WHITE, menuAlpha));

            // Botão Quit
            int textWidth3 = MeasureText("Quit", fontSize);
            float buttonQuitX = ((GetScreenWidth() - textWidth3) / 2);
            float buttonQuitY = (GetScreenHeight() / 2) + 120;
            Rectangle quitButton = {buttonQuitX, buttonQuitY, (float)textWidth3, (float)fontSize};
            if (CheckCollisionPointRec(mousePos, quitButton)) {
                DrawText("Quit", buttonQuitX, buttonQuitY, fontSize, Fade(YELLOW, menuAlpha));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) CloseWindow();
            } else DrawText("Quit", buttonQuitX, buttonQuitY, fontSize, Fade(WHITE, menuAlpha));
            
            DrawTextureEx(mouse, Vector2{mousePos.x - 8, mousePos.y - 8}, 0.0f, 2.0f, WHITE);
        }
        
////////////////////////////////////////////////////////////////////////
// OPTIONS //
////////////////////////////////////////////////////////////////////////
if (currentState == OPTIONS)
{
    // Carregando fundo
    DrawTexturePro(
        backgroundmenu,
        Rectangle{
            0,
            0,
            (float)backgroundmenu.width,
            (float)backgroundmenu.height
        }, // source
        Rectangle{
            0,
            0,
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        }, // dest
        Vector2{0, 0},      // origin
        0.0f,               // rotation
        Fade(WHITE, menuAlpha)
    ); // cor

    // Slider: só passa NULL para textRight
    GuiSlider(
        (Rectangle){
            (float)GetScreenWidth() / 2 - 100,
            (float)GetScreenHeight() / 2,
            216,
            16
        },
        NULL,
        NULL,
        &master_volume,
        0.0f,
        1.0f
    );

    // Mostra o valor fora do slider
    DrawText(
        TextFormat("Volume: %d%%", (int)(master_volume * 100)),
        (float)GetScreenWidth() / 2 - 60,
        (float)GetScreenHeight() / 2 + 50,
        20,
        WHITE
    );

    SetMasterVolume(master_volume);

    // Botão Back
    int textWidth3 = MeasureText("Back", 35.0f);
    float buttonQuitX = (GetScreenWidth() - textWidth3) / 2;
    float buttonQuitY = (GetScreenHeight() / 2) + 120;

    DrawText(
        "Back",
        buttonQuitX,
        buttonQuitY,
        35.0f,
        Fade(WHITE, menuAlpha)
    );

    Rectangle quitButton = {
        buttonQuitX,
        buttonQuitY,
        (float)textWidth3,
        35.0f
    };

    if (CheckCollisionPointRec(mousePos, quitButton))
    {
        // mouse por cima → amarelo
        DrawText(
            "Back",
            buttonQuitX,
            buttonQuitY,
            35.0f,
            Fade(YELLOW, menuAlpha)
        );

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (game1 == true)
                currentState = GAME1;
            else if (game2 == true)
                currentState = GAME2;
            else
                currentState = MENU;
        }
    }
    else
    {
        // normal
        DrawText(
            "Back",
            buttonQuitX,
            buttonQuitY,
            35.0f,
            Fade(WHITE, menuAlpha)
        );
    }

    // Mouse
    DrawTextureEx(
        mouse,
        Vector2{mousePos.x - 8, mousePos.y - 8},
        0.0f,
        2.0f,
        WHITE
    );
}

///////////////////////////////////////////
// GAME1
if (currentState == GAME1) {
	Vector2 playerCenter = { rplayer.x + rplayer.width / 2, rplayer.y + rplayer.height / 2 };
        float angle = atan2f(mousePos.y - playerCenter.y, mousePos.x - playerCenter.x);
    game1 = true;
    
    if (IsKeyPressed(KEY_F1)) debugMode = !debugMode;
    if (IsKeyPressed(KEY_ESCAPE)) currentState = OPTIONS;

    // Background
    DrawTexturePro(backgroundgame1,
                   Rectangle{0,0,(float)backgroundgame1.width,(float)backgroundgame1.height},
                   Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                   Vector2{0,0}, 0.0f, WHITE);

    // Gun pickup
    if (!gunPicked) {
        DrawTextureEx(gun1, Vector2{rgun1.x, rgun1.y}, 0.0f, 2.0f, WHITE);
        if (CheckCollisionRecs(rplayer, rgun1)) {
            DrawText("(E)", rgun1.x + 20, rgun1.y - 35, 20, WHITE);
            if (IsKeyPressed(KEY_E)) { gunPicked = true; hasGun = true; }
        }
    }

    // Player

    if (hasGun) {
        DrawTexturePro(playergun1,
                       Rectangle{0,0,(float)player.width,(float)player.height},
                       Rectangle{playerCenter.x, playerCenter.y, rplayer.width, rplayer.height},
                       Vector2{rplayer.width/2,rplayer.height/2}, angle*180.0f/PI, WHITE);

        // Tiro
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Bullet b;
    b.position = playerCenter;
    float bulletSpeed = 1500.0f;
    b.speed = { cosf(angle)*bulletSpeed, sinf(angle)*bulletSpeed };
    b.active = true;
    b.currentFrame = 0;
    b.frameTime = 0.0f;
    b.angle = angle; 
    bullets.push_back(b);
    PlaySound(shot);
    
}
    } else {
        DrawTexturePro(player,
                       Rectangle{0,0,(float)player.width,(float)player.height},
                       Rectangle{playerCenter.x, playerCenter.y, rplayer.width, rplayer.height},
                       Vector2{rplayer.width/2,rplayer.height/2}, angle*180.0f/PI, WHITE);
    }

    // Movimento
    float speed = 200.0f;
    if (IsKeyDown(KEY_W)) rplayer.y -= speed*dt;
    if (IsKeyDown(KEY_S)) rplayer.y += speed*dt;
    if (IsKeyDown(KEY_A)) rplayer.x -= speed*dt;
    if (IsKeyDown(KEY_D)) rplayer.x += speed*dt;

    // Limitar Player
    if (rplayer.x < 0) rplayer.x = 0;
    if (rplayer.y < 100) rplayer.y = 100;
    if (rplayer.x + rplayer.width > GetScreenWidth()) rplayer.x = GetScreenWidth() - rplayer.width;
    if (rplayer.y + rplayer.height > GetScreenHeight()) rplayer.y = GetScreenHeight() - rplayer.height;

    // Colisão com porta GAME2
    if (CheckCollisionRecs(rplayer, rdoor1)) {
        DrawText("(E)", rdoor1.x + 50, rdoor1.y - 10, 20, WHITE);
        if (IsKeyPressed(KEY_E)) {
            dooropen = true;
            currentState = GAME2;
        }
    }

    // Atualiza e desenha tiros
    for (auto &b : bullets) {
        if (!b.active) continue;
        b.position.x += b.speed.x * dt;
        b.position.y += b.speed.y * dt;

        b.frameTime += dt;
        if (b.frameTime >= 0.05f) {
            b.frameTime = 0.0f;
            b.currentFrame++;
            if (b.currentFrame >= bulletFrames) b.currentFrame = 0;
        }

        if (b.position.x < 0 || b.position.x > GetScreenWidth() ||
            b.position.y < 0 || b.position.y > GetScreenHeight()) b.active = false;

        Rectangle bulletRec = { float(b.currentFrame*bulletFrameWidth), 0.0f, float(bulletFrameWidth), float(bulletFrameHeight) };
Vector2 bulletPos = { b.position.x - bulletFrameWidth/2.0f, b.position.y - bulletFrameHeight/2.0f };
Vector2 origin = { bulletFrameWidth/2.0f, bulletFrameHeight/2.0f };

DrawTexturePro(bulletTexture, bulletRec, Rectangle{bulletPos.x, bulletPos.y, (float)bulletFrameWidth, (float)bulletFrameHeight}, origin, b.angle*180.0f/PI, WHITE);

    }

    // Debug
    if (debugMode) {
        DrawRectangleLinesEx(rplayer, 2, RED);
        DrawCircle(playerCenter.x, playerCenter.y, 5, BLUE);
        if (!gunPicked) DrawRectangleLinesEx(rgun1, 2, RED);
        DrawRectangleLinesEx(rdoor1, 2, RED);
    }
    
    DrawTextureEx(mouse, Vector2{mousePos.x - 8, mousePos.y - 8}, 0.0f, 2.0f, WHITE);
}
///////////////////////////////////////////////////////
// GAME2
if (currentState == GAME2) {
	game1 = false;
    game2 = true;
    dooropen = false;
    Vector2 playerCenter = {
        rplayer.x + rplayer.width / 2,
        rplayer.y + rplayer.height / 2
    };

    float angle = atan2f(
        mousePos.y - playerCenter.y,
        mousePos.x - playerCenter.x
    );
    

    if (!teleport1done) {
        rplayer = {609, 611, rplayer.width, rplayer.height};
        teleport1done = true;
    }

    // Background
    DrawTexturePro(backgroundgame2,
                   Rectangle{0,0,(float)backgroundgame2.width,(float)backgroundgame2.height},
                   Rectangle{0,0,(float)GetScreenWidth(),(float)GetScreenHeight()},
                   Vector2{0,0}, 0.0f, WHITE);

    // Porta (rdoor2)
    DrawTexturePro(dooriron,
                   Rectangle{0,0,(float)dooriron.width,(float)dooriron.height},
                   Rectangle{rdoor2.x,rdoor2.y,rdoor2.width,rdoor2.height},
                   Vector2{0,0}, 0.0f, WHITE);

    // Colisão com porta GAME3
    if (CheckCollisionRecs(rplayer, rdoor2)) {
        if (!enemiesdefeated) DrawText("(E)", rdoor2.x + 48, rdoor2.y, 20, RED);
        else {
            DrawText("(E)", rdoor2.x + 48, rdoor2.y, 20, WHITE);
            if (IsKeyPressed(KEY_E)) {
                dooropen = true;
                currentState = GAME3;
            }
        }
    }
    


    
            // Tiro
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Bullet b;
    b.position = playerCenter;
    float bulletSpeed = 1500.0f;
    b.speed = { cosf(angle)*bulletSpeed, sinf(angle)*bulletSpeed };
    b.active = true;
    b.currentFrame = 0;
    b.frameTime = 0.0f;
    b.angle = angle; 
    bullets.push_back(b);
    PlaySound(shot);
    } 

    // Player
if (hasGun) {
    DrawTexturePro(playergun1,
        Rectangle{0,0,(float)player.width,(float)player.height},
        Rectangle{playerCenter.x, playerCenter.y, rplayer.width, rplayer.height},
        Vector2{rplayer.width/2, rplayer.height/2},
        angle * 180.0f / PI,
        WHITE
    );
} else {
    DrawTexturePro(player,
        Rectangle{0,0,(float)player.width,(float)player.height},
        Rectangle{playerCenter.x, playerCenter.y, rplayer.width, rplayer.height},
        Vector2{rplayer.width/2, rplayer.height/2},
        angle * 180.0f / PI,
        WHITE
    );
}


    // Movimento
    float speed = 200.0f;
    if (IsKeyDown(KEY_W)) rplayer.y -= speed*dt;
    if (IsKeyDown(KEY_S)) rplayer.y += speed*dt;
    if (IsKeyDown(KEY_A)) rplayer.x -= speed*dt;
    if (IsKeyDown(KEY_D)) rplayer.x += speed*dt;

    // Limitar Player
    if (rplayer.x < 0) rplayer.x = 0;
    if (rplayer.y < 100) rplayer.y = 100;
    if (rplayer.x + rplayer.width > GetScreenWidth()) rplayer.x = GetScreenWidth() - rplayer.width;
    if (rplayer.y + rplayer.height > GetScreenHeight()) rplayer.y = GetScreenHeight() - rplayer.height;
    
  // Atualiza e desenha tiros
    for (auto &b : bullets) {
        if (!b.active) continue;
        b.position.x += b.speed.x * dt;
        b.position.y += b.speed.y * dt;

        b.frameTime += dt;
        if (b.frameTime >= 0.05f) {
            b.frameTime = 0.0f;
            b.currentFrame++;
            if (b.currentFrame >= bulletFrames) b.currentFrame = 0;
        }

        if (b.position.x < 0 || b.position.x > GetScreenWidth() ||
            b.position.y < 0 || b.position.y > GetScreenHeight()) b.active = false;

        Rectangle bulletRec = { float(b.currentFrame*bulletFrameWidth), 0.0f, float(bulletFrameWidth), float(bulletFrameHeight) };
Vector2 bulletPos = { b.position.x - bulletFrameWidth/2.0f, b.position.y - bulletFrameHeight/2.0f };
Vector2 origin = { bulletFrameWidth/2.0f, bulletFrameHeight/2.0f };

DrawTexturePro(bulletTexture, bulletRec, Rectangle{bulletPos.x, bulletPos.y, (float)bulletFrameWidth, (float)bulletFrameHeight}, origin, b.angle*180.0f/PI, WHITE);
}

	// Inimigos
	spawnzombies(zombies, 10);

	
for (auto &z : zombies) {
    Vector2 direction = {
        playerCenter.x - z.position.x,
        playerCenter.y - z.position.y
    };

    float distance = sqrtf(direction.x*direction.x +
                            direction.y*direction.y);

    if (distance > 5.0f) {
        direction.x /= distance;
        direction.y /= distance;

        z.position.x += direction.x * z.speed * dt;
        z.position.y += direction.y * z.speed * dt;
    }

    DrawTextureEx(
    zombie1,
    z.position,
    0.0f,     // rotação
    5.0f,    // escala
    WHITE
);
}

	

    // Debug
    if (debugMode) {
        DrawRectangleLinesEx(rplayer, 2, RED);
        DrawCircle(playerCenter.x, playerCenter.y, 5, BLUE);
        DrawRectangleLinesEx(rdoor2, 2, RED);
        if (!gunPicked) DrawRectangleLinesEx(rgun1, 2, RED);
    }

    DrawTextureEx(mouse,
        Vector2{mousePos.x - 8, mousePos.y - 8},
        0.0f,
        2.0f,
        WHITE
    );
}

    EndDrawing();
} // End while

CloseWindow();
return 0;
} // End main



