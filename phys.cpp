#include <raylib.h>
#include <vector>
#include <iostream>

const int FPS = 60;
const int screen_width = 800, screen_height = 600;

class Vec2 {
public:
  float x, y;

  Vec2(float x, float y) : x{x}, y{y} {}

  Vec2 operator+(const Vec2 rhs) {
    return Vec2(x + rhs.x, y + rhs.y);
  }

  Vec2 operator-(const Vec2 rhs) {
    return Vec2(x - rhs.x, y - rhs.y);
  }

  Vec2 operator-() {
    return Vec2(-x, -y);
  }


  Vec2 operator+=(const Vec2 rhs) {
    *this = *this + rhs;
    return *this;
  }

  Vec2 operator/(const float lambda) {
    return Vec2(x / lambda, y / lambda);
  }

  void print() {
    std::cout << "(" << x << ", " << y << ")\n" << std::flush;
  }

  Vec2 discrete() {
    return *this / FPS;
  }
};

class PointMass {
public:
  Vec2 pos, vel = Vec2(0, 0);
  float mass;
  int radius = 10;
  Color color = GREEN;

  PointMass(Vec2 pos, float mass) : pos{pos}, mass{mass} {}
  PointMass(Vec2 pos, Vec2 vel, float mass) : pos{pos}, vel{vel}, mass{mass} {}

  void draw() {
    DrawCircle(pos.x, pos.y, radius, color);
  }
};


// check for collision. if there is, then return new vel vector
Vec2 collide(Vec2& pos, Vec2& vel) {
  if (pos.x > screen_width or pos.x < 0) {
    return Vec2(-vel.x, vel.y);
  }
  if (pos.y > screen_height or pos.y < 0) {
    return Vec2(vel.x, -vel.y);
  }
  return vel;
}


class World {
  Vec2 gravity = Vec2(0, 100);

public:
  std::vector<PointMass> objects;

  World() : objects{} {}

  void add_object(PointMass p) {
    objects.push_back(p);
  }

  void draw() {
    for (auto o : objects) {
      o.draw();
    }
  }

  void step() {
    for (auto &p : objects) {
      p.pos += p.vel.discrete();
      p.vel += gravity.discrete();
      p.vel = collide(p.pos, p.vel);
    }
  }
};


int main() {
  InitWindow(screen_width, screen_height, "phys");
  SetTargetFPS(FPS);

  World world;
  //world.add_object(PointMass(Vec2(400, 300), Vec2(0, 0), 1.0));
  world.add_object(PointMass(Vec2(50, 550), Vec2(200, -200), 1.0));
  world.step();


  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);
    world.draw();

    EndDrawing();

    world.objects[0].pos.print();

    world.step();
  }

  CloseWindow();
  
  return 0;
}
