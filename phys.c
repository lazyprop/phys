#include <raylib.h>


int main() {
  const int screen_width = 800, screen_height = 600;
  InitWindow(screen_width, screen_height, "phys");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);
    DrawCircle(400, 300, 10, GREEN);

    EndDrawing();
  }

  CloseWindow();
  
  return 0;
}
