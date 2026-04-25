#include <allegro.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

const int COIN_COUNT = 5;

int main() {
    allegro_init();
    install_keyboard();
    install_timer();

    set_color_depth(32);

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Could not set full screen mode!");
        return 1;
    }
    set_window_title("Desert Dash");

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    BITMAP *background = load_bitmap("images/background_2.bmp", NULL);

    BITMAP *heart1 = load_bitmap("images/heart.bmp", NULL);
    BITMAP *heart2 = load_bitmap("images/heart.bmp", NULL);
    BITMAP *heart3 = load_bitmap("images/heart.bmp", NULL);

    BITMAP *camal1 = load_bitmap("images/camal1.bmp", NULL);
    BITMAP *camal2 = load_bitmap("images/camal2.bmp", NULL);
    BITMAP *camal3 = load_bitmap("images/camal3.bmp", NULL);

    BITMAP *cactus1 = load_bitmap("images/cactus1.bmp", NULL);
    BITMAP *cactus2 = load_bitmap("images/cactus2.bmp", NULL);

    BITMAP *coin = load_bitmap("images/coin.bmp", NULL);

    if (!background || !heart1 || !heart2 || !heart3 ||
        !camal1 || !camal2 || !camal3 ||
        !cactus1 || !cactus2 || !coin) {
        allegro_message("Failed to load images!");
        return 1;
    }

    srand(time(NULL));

    // ======================
    // GAME STATE
    // ======================
    bool gameStarted = false;
    bool gameOver = false;

    int score = 0;
    int lives = 3;

    // ======================
    // CAMEL
    // ======================
    int camalX = -37;
    int camalY = SCREEN_H - 240;

    int velocityY = 0;
    bool isJumping = false;

    int frame = 0;
    int frameDelay = 0;

    const int groundY = SCREEN_H - 240;

    // ======================
    // CACTUS
    // ======================
    int cactusX1 = SCREEN_W;
    int cactusX2 = SCREEN_W + 400;
    int cactusX3 = SCREEN_W + 800;

    int cactusY = SCREEN_H - 220;
    int cactusSpeed = 10;
    int cactusSpacing = 550;

    int cactusType1 = 1;
    int cactusType2 = 2;
    int cactusType3 = 1;

    // ======================
    // COINS
    // ======================
    int coinX[COIN_COUNT];
    int coinY[COIN_COUNT];

    int coinSpeed = 10;

    for (int i = 0; i < COIN_COUNT; i++) {
        coinX[i] = SCREEN_W + rand() % 800;
        coinY[i] = (rand() % 2 == 0) ? SCREEN_H - 120 : SCREEN_H - 300;
    }

    // ======================
    // EFFECTS
    // ======================
    int blinkTimer = 0;
    bool isBlinking = false;

    int shakeTimer = 0;
    int shakeIntensity = 5;

    while (!key[KEY_ESC]) {

        // ======================
        // GAME OVER SCREEN
        // ======================
        if (gameOver) {

            char finalScore[50];
            sprintf(finalScore, "FINAL SCORE: %d", score);

            textout_centre_ex(buffer, font, finalScore,
                SCREEN_W / 2, SCREEN_H / 2 - 80,
                makecol(255, 255, 0), 4);

            textout_centre_ex(buffer, font,
                "PRESS ENTER TO PLAY AGAIN",
                SCREEN_W / 2, SCREEN_H / 2,
                makecol(255, 255, 255), -1);

            textout_centre_ex(buffer, font,
                "PRESS ESC TO EXIT",
                SCREEN_W / 2, SCREEN_H / 2 + 40,
                makecol(255, 255, 255), -1);

            blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

            if (key[KEY_ENTER]) {

                // Reset game
                score = 0;
                lives = 3;

                camalX = -37;
                camalY = SCREEN_H - 240;
                velocityY = 0;

                cactusX1 = SCREEN_W;
                cactusX2 = SCREEN_W + 400;
                cactusX3 = SCREEN_W + 800;

                for (int i = 0; i < COIN_COUNT; i++) {
                    coinX[i] = SCREEN_W + rand() % 800;
                    coinY[i] = (rand() % 2 == 0) ? SCREEN_H - 120 : SCREEN_H - 300;
                }

                gameOver = false;
                clear_keybuf();
            }

            rest(16);
            continue;
        }

        // ======================
        // SHAKE
        // ======================
        int offsetX = 0;
        int offsetY = 0;

        if (shakeTimer > 0) {
            shakeTimer--;
            offsetX = (rand() % (shakeIntensity * 2)) - shakeIntensity;
            offsetY = (rand() % (shakeIntensity * 2)) - shakeIntensity;
        }

        stretch_blit(background, buffer,
            0, 0, background->w, background->h,
            offsetX, offsetY,
            SCREEN_W, SCREEN_H);

        if (!gameStarted) {

            textout_centre_ex(buffer, font,
                "Press ENTER To Start",
                SCREEN_W / 2, SCREEN_H / 2,
                makecol(255, 255, 255), -1);

            if (key[KEY_ENTER]) {
                gameStarted = true;
                clear_keybuf();
            }
        }
        else {

            if (lives <= 0) {
                gameOver = true;
                continue;
            }

            // Score
            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            textout_ex(buffer, font, scoreText, 20, 20, makecol(255,255,255), -1);

            // Hearts
            if (lives >= 1) stretch_sprite(buffer, heart1, SCREEN_W - 150, 20, 40, 40);
            if (lives >= 2) stretch_sprite(buffer, heart2, SCREEN_W - 100, 20, 40, 40);
            if (lives >= 3) stretch_sprite(buffer, heart3, SCREEN_W - 50, 20, 40, 40);

            // Jump
            if (key[KEY_SPACE] && !isJumping) {
                velocityY = -19;
                isJumping = true;
            }

            camalY += velocityY;
            velocityY += 1;

            if (camalY >= groundY) {
                camalY = groundY;
                velocityY = 0;
                isJumping = false;
            }

            // Animation
            frameDelay++;
            if (frameDelay > 5) {
                frame++;
                if (frame > 2) frame = 0;
                frameDelay = 0;
            }

            BITMAP *currentCamal = camal1;
            if (frame == 1) currentCamal = camal2;
            if (frame == 2) currentCamal = camal3;

            if (!isBlinking || (blinkTimer / 5) % 2 == 0) {
                stretch_sprite(buffer, currentCamal,
                    camalX + offsetX, camalY + offsetY, 250, 250);
            }

            // CACTUS
            cactusX1 -= cactusSpeed;
            cactusX2 -= cactusSpeed;
            cactusX3 -= cactusSpeed;

            if (cactusX1 < -100) cactusX1 = cactusX3 + cactusSpacing;
            if (cactusX2 < -100) cactusX2 = cactusX1 + cactusSpacing;
            if (cactusX3 < -100) cactusX3 = cactusX2 + cactusSpacing;

            BITMAP *c1 = (cactusType1 == 1) ? cactus1 : cactus2;
            BITMAP *c2 = (cactusType2 == 1) ? cactus1 : cactus2;
            BITMAP *c3 = (cactusType3 == 1) ? cactus1 : cactus2;

            stretch_sprite(buffer, c1, cactusX1 + offsetX, cactusY + offsetY, 220, 300);
            stretch_sprite(buffer, c2, cactusX2 + offsetX, cactusY + offsetY, 220, 300);
            stretch_sprite(buffer, c3, cactusX3 + offsetX, cactusY + offsetY, 220, 300);

            // COLLISION (tight)
            auto hit = [&](int cx, int cy) -> bool {
                int cL = camalX + 70;
                int cR = camalX + 150;
                int cT = camalY + 60;
                int cB = camalY + 190;

                int xL = cx + 50;
                int xR = cx + 140;
                int xT = cy + 60;
                int xB = cy + 240;

                return !(cR < xL || cL > xR || cB < xT || cT > xB);
            };

            if ((hit(cactusX1, cactusY) || hit(cactusX2, cactusY) || hit(cactusX3, cactusY))
                && !isBlinking) {

                lives--;
                isBlinking = true;
                blinkTimer = 60;

                shakeTimer = 15;
                shakeIntensity = 5;
            }

            if (isBlinking) blinkTimer--;
            if (blinkTimer <= 0) isBlinking = false;

            // COINS + SCORE
            for (int i = 0; i < COIN_COUNT; i++) {

                coinX[i] -= coinSpeed;

                if (coinX[i] < -50) {
                    coinX[i] = SCREEN_W + rand() % 800;
                    coinY[i] = (rand() % 2 == 0) ? SCREEN_H - 120 : SCREEN_H - 300;
                }

                stretch_sprite(buffer, coin,
                    coinX[i] + offsetX,
                    coinY[i] + offsetY, 50, 50);

                if (camalX < coinX[i] + 40 &&
                    camalX + 200 > coinX[i] &&
                    camalY < coinY[i] + 40 &&
                    camalY + 200 > coinY[i]) {

                    score += 10;

                    coinX[i] = SCREEN_W + rand() % 800;
                    coinY[i] = (rand() % 2 == 0) ? SCREEN_H - 120 : SCREEN_H - 300;
                }
            }
        }

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(16);
    }

    destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(heart1);
    destroy_bitmap(heart2);
    destroy_bitmap(heart3);
    destroy_bitmap(camal1);
    destroy_bitmap(camal2);
    destroy_bitmap(camal3);
    destroy_bitmap(cactus1);
    destroy_bitmap(cactus2);
    destroy_bitmap(coin);

    return 0;
}
END_OF_MAIN()
