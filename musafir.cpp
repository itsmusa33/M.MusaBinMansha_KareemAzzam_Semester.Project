#include "raylib.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const Color green= {0, 102, 51, 255};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Musafir - Pakistan Travel Guide"); //Initialize window
    SetTargetFPS(60);
    // Main loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(green);
        DrawText("MUSAFIR", 380, 300, 64, WHITE);
        DrawText("Pakistan Travel Guide", 380, 380, 24, Color{200, 255, 200, 255});
        
        EndDrawing();
    }
    
    // Cleanup
    CloseWindow();
    return 0;
}
