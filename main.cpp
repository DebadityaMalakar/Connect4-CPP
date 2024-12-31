#include <raylib.h>
#include <cstdio>

bool checkWin(int row, int col);

int grid[4][8] = {{0}};
int currentPlayer = 0;
bool gameStarted = false;
bool gameWon = false;
int player1Score = 0;
int player2Score = 0;
int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

// Keep the existing dropPiece and checkWin functions unchanged
void dropPiece(int col) {
    for (int row = 3; row >= 0; row--) {
        if (grid[row][col] == 0) {
            grid[row][col] = currentPlayer == 1 ? 3 : 2;
            if (checkWin(row, col)) {
                gameWon = true;
            } else {
                currentPlayer = (currentPlayer == 1) ? 2 : 1;
            }
            break;
        }
    }
}

bool checkWin(int row, int col) {
    int color = grid[row][col];
    for (int i = 0; i < 4; i++) {
        int count = 1;
        int dr = directions[i][0];
        int dc = directions[i][1];

        for (int j = 1; j < 4; j++) {
            int r = row + dr * j;
            int c = col + dc * j;
            if (r >= 0 && r < 4 && c >= 0 && c < 8 && grid[r][c] == color) count++;
            else break;
        }

        for (int j = 1; j < 4; j++) {
            int r = row - dr * j;
            int c = col - dc * j;
            if (r >= 0 && r < 4 && c >= 0 && c < 8 && grid[r][c] == color) count++;
            else break;
        }

        if (count >= 4) return true;
    }
    return false;
}

// Add animation structure
typedef struct {
    float scale;
    float alpha;
} ButtonAnimation;

int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Connect 4");
    SetTargetFPS(60);

    // UI Constants
    const int cellWidth = 80;
    const int cellHeight = 80;
    const int gridStartX = (screenWidth - 8 * cellWidth) / 2;
    const int gridStartY = (screenHeight - 4 * cellHeight) / 2;

    // Button animations
    ButtonAnimation playAnim = {1.0f, 1.0f};
    ButtonAnimation exitAnim = {1.0f, 1.0f};
    ButtonAnimation player1Anim = {1.0f, 1.0f};
    ButtonAnimation player2Anim = {1.0f, 1.0f};
    ButtonAnimation resetAnim = {1.0f, 1.0f};
    ButtonAnimation homeAnim = {1.0f, 1.0f};

    // Define buttons with consistent styling
    Rectangle playButton = {screenWidth/2 - 100, screenHeight/2 - 60, 200, 50};
    Rectangle exitButton = {screenWidth/2 - 100, screenHeight/2 + 10, 200, 50};
    Rectangle player1Button = {gridStartX - 120, gridStartY - 50, 100, 40};
    Rectangle player2Button = {gridStartX + 600, gridStartY - 50, 100, 40};
    Rectangle resetButton = {screenWidth/2 - 100, screenHeight/2 + 50, 200, 50};
    Rectangle homeButton = {screenWidth/2 - 100, screenHeight/2 + 110, 200, 50};

    bool onHomeScreen = true;
    bool onGameScreen = false;
    float titleScale = 1.0f;
    bool titleScaleIncreasing = true;

    while (!WindowShouldClose())
    {
        // Update animations
        if (titleScaleIncreasing) {
            titleScale += 0.001f;
            if (titleScale >= 1.05f) titleScaleIncreasing = false;
        } else {
            titleScale -= 0.001f;
            if (titleScale <= 0.95f) titleScaleIncreasing = true;
        }

        if (onHomeScreen) {
            // Home screen button animations
            if (CheckCollisionPointRec(GetMousePosition(), playButton)) {
                playAnim.scale = 1.1f;
                playAnim.alpha = 0.9f;
            } else {
                playAnim.scale = 1.0f;
                playAnim.alpha = 1.0f;
            }

            if (CheckCollisionPointRec(GetMousePosition(), exitButton)) {
                exitAnim.scale = 1.1f;
                exitAnim.alpha = 0.9f;
            } else {
                exitAnim.scale = 1.0f;
                exitAnim.alpha = 1.0f;
            }

            // Home screen input handling
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(GetMousePosition(), playButton)) {
                    onHomeScreen = false;
                    onGameScreen = true;
                    gameStarted = false;
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 8; j++)
                            grid[i][j] = 0;
                } else if (CheckCollisionPointRec(GetMousePosition(), exitButton)) {
                    CloseWindow();
                    return 0;
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw animated title
            const char* title = "CONNECT 4";
            Vector2 titleSize = MeasureTextEx(GetFontDefault(), title, 60, 2);
            DrawTextPro(GetFontDefault(), title,
                       (Vector2){screenWidth/2, screenHeight/3},
                       (Vector2){titleSize.x/2, titleSize.y/2},
                       0, 60, 2, DARKBLUE);

            // Draw buttons with animations
            Color playColor = ColorAlpha(BLUE, playAnim.alpha);
            Color exitColor = ColorAlpha(RED, exitAnim.alpha);

            DrawRectanglePro(
                (Rectangle){playButton.x + playButton.width/2, playButton.y + playButton.height/2,
                           playButton.width * playAnim.scale, playButton.height * playAnim.scale},
                (Vector2){playButton.width/2 * playAnim.scale, playButton.height/2 * playAnim.scale},
                0, playColor);
            DrawText("PLAY", playButton.x + (playButton.width - MeasureText("PLAY", 30))/2,
                    playButton.y + (playButton.height - 30)/2, 30, WHITE);

            DrawRectanglePro(
                (Rectangle){exitButton.x + exitButton.width/2, exitButton.y + exitButton.height/2,
                           exitButton.width * exitAnim.scale, exitButton.height * exitAnim.scale},
                (Vector2){exitButton.width/2 * exitAnim.scale, exitButton.height/2 * exitAnim.scale},
                0, exitColor);
            DrawText("EXIT", exitButton.x + (exitButton.width - MeasureText("EXIT", 30))/2,
                    exitButton.y + (exitButton.height - 30)/2, 30, WHITE);

            EndDrawing();
        }
        else if (onGameScreen) {
            // Game screen input handling
            if (!gameStarted) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (CheckCollisionPointRec(GetMousePosition(), player1Button)) {
                        currentPlayer = 1;
                        gameStarted = true;
                    } else if (CheckCollisionPointRec(GetMousePosition(), player2Button)) {
                        currentPlayer = 2;
                        gameStarted = true;
                    }
                }
            }
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !gameWon) {
                int col = (GetMouseX() - gridStartX) / cellWidth;
                if (col >= 0 && col < 8) dropPiece(col);
            }

            // Handle reset and home buttons
            if (gameWon && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(GetMousePosition(), resetButton)) {
                    gameStarted = false;
                    gameWon = false;
                    currentPlayer = 0;
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 8; j++)
                            grid[i][j] = 0;
                }
                else if (CheckCollisionPointRec(GetMousePosition(), homeButton)) {
                    onHomeScreen = true;
                    onGameScreen = false;
                    gameStarted = false;
                    gameWon = false;
                    currentPlayer = 0;
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Draw game title
            DrawText("CONNECT 4", screenWidth/2 - MeasureText("CONNECT 4", 40)/2, 20, 40, DARKGRAY);

            // Draw player indicators
            DrawText("Player 1", gridStartX - 110, gridStartY - 80, 24, BLUE);
            DrawText("Player 2", gridStartX + 560, gridStartY - 80, 24, GREEN);

            // Draw current player indicator
            if (gameStarted && !gameWon) {
                const char* turnText = TextFormat("Player %d's Turn", currentPlayer);
                DrawText(turnText, screenWidth/2 - MeasureText(turnText, 30)/2, gridStartY - 80, 30,
                        currentPlayer == 1 ? BLUE : GREEN);
            }

            // Draw player selection buttons
            if (!gameStarted) {
                DrawRectangleRec(player1Button, BLUE);
                DrawRectangleRec(player2Button, GREEN);
                DrawText("Select", player1Button.x + 20, player1Button.y + 10, 20, WHITE);
                DrawText("Select", player2Button.x + 20, player2Button.y + 10, 20, WHITE);
            }

            // Draw grid with enhanced visuals
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 8; col++) {
                    int x = gridStartX + col * cellWidth;
                    int y = gridStartY + row * cellHeight;

                    // Draw cell background
                    DrawRectangle(x, y, cellWidth, cellHeight, LIGHTGRAY);
                    DrawRectangleLines(x, y, cellWidth, cellHeight, DARKGRAY);

                    // Draw pieces with shadow effect
                    if (grid[row][col] > 0) {
                        Color pieceColor = grid[row][col] == 3 ? BLUE : GREEN;
                        // Shadow
                        DrawCircle(x + cellWidth/2 + 2, y + cellHeight/2 + 2, cellWidth/3, ColorAlpha(BLACK, 0.3f));
                        // Piece
                        DrawCircle(x + cellWidth/2, y + cellHeight/2, cellWidth/3, pieceColor);
                        // Highlight
                        DrawCircle(x + cellWidth/2, y + cellHeight/2, cellWidth/3 - 5, ColorAlpha(WHITE, 0.3f));
                    } else {
                        // Empty cell
                        DrawCircle(x + cellWidth/2, y + cellHeight/2, cellWidth/3, WHITE);
                    }
                }
            }

            // Draw win screen
            if (gameWon) {
                DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.7f));
                
                const char* winnerText = TextFormat("Player %d Wins!", currentPlayer);
                DrawText(winnerText, screenWidth/2 - MeasureText(winnerText, 50)/2,
                        screenHeight/2 - 100, 50, WHITE);

                // Calculate and display score
                int winnerScore = 0;
                for (int row = 0; row < 4; row++)
                    for (int col = 0; col < 8; col++)
                        if (grid[row][col] == (currentPlayer == 1 ? 3 : 2))
                            winnerScore++;
                
                const char* scoreText = TextFormat("Score: %d", winnerScore * 2);
                DrawText(scoreText, screenWidth/2 - MeasureText(scoreText, 40)/2,
                        screenHeight/2 - 20, 40, WHITE);

                // Draw buttons with hover effects
                Color resetColor = CheckCollisionPointRec(GetMousePosition(), resetButton) ?
                                 ColorAlpha(DARKGRAY, 0.8f) : DARKGRAY;
                Color homeColor = CheckCollisionPointRec(GetMousePosition(), homeButton) ?
                                ColorAlpha(DARKGRAY, 0.8f) : DARKGRAY;

                DrawRectangleRec(resetButton, resetColor);
                DrawRectangleRec(homeButton, homeColor);
                
                DrawText("Restart Game", resetButton.x + (resetButton.width - MeasureText("Restart Game", 20))/2,
                        resetButton.y + 15, 20, WHITE);
                DrawText("Go to Home", homeButton.x + (homeButton.width - MeasureText("Go to Home", 20))/2,
                        homeButton.y + 15, 20, WHITE);
            }

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}