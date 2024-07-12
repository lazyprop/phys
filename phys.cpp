#include <raylib.h>
#include <vector>
#include <iostream>
#include <cmath>

const int FPS = 60;
const int screen_width = 1800, screen_height = 900;

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

  Vec2 operator*(const float lambda) {
    return Vec2(x * lambda, y * lambda);
  }

  void print() {
    std::cout << "(" << x << ", " << y << ")\n" << std::flush;
  }

  Vec2 discrete() {
    return *this / FPS;
  }

  float norm() {
    return sqrt(x*x + y*y);
  }


  friend Vec2 operator*(const double lambda, const Vec2 &v);
};

Vec2 operator*(const double lambda, const Vec2 &v) {
  return Vec2(v.x * lambda, v.y * lambda);
}

class PointMass {
public:
  Vec2 pos, vel = Vec2(0, 0);
  float mass;
  int radius = 10;
  Color color = GREEN;

  PointMass(Vec2 pos, float mass) : pos{pos}, mass{mass} {}
  PointMass(Vec2 pos, float mass, Color color) : pos{pos}, mass{mass}, color{color} {}
  PointMass(Vec2 pos, Vec2 vel, float mass) : pos{pos}, vel{vel}, mass{mass} {}
  PointMass(Vec2 pos, Vec2 vel, float mass, Color color)
    : pos{pos}, vel{vel}, mass{mass}, color{color} {}

  void draw() {
    DrawCircle(pos.x, pos.y, radius, color);
  }

  bool operator==(const PointMass&) const = default;
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


Vec2 gravity(PointMass &p, PointMass &q) {
  float G = 1e3;
  float dist = (q.pos - p.pos).norm();
  Vec2 force = (G * p.mass * q.mass * (q.pos - p.pos)) / (dist * dist * dist);
  Vec2 acc = force / p.mass;
  return acc;
}

class World {
  //Vec2 gravity = Vec2(0, 200);
  Vec2 world_gravity = Vec2(0, 100);
  std::vector<PointMass> objects;

public:
  float energy; // total mechanical energy

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
    std::vector<PointMass> new_obj(objects);

    for (int i = 0; i < objects.size(); i++) {
      auto p = &new_obj[i];
      p->pos += p->vel.discrete();

      for (int j = 0; j < objects.size(); j++) {
        if (i == j) continue;
        auto q = &objects[j];
        p->vel += gravity(*p, *q).discrete();
      }

      p->vel = collide(p->pos, p->vel);
    }

    objects = new_obj;
  }

};


World two_body() {
  World world;
  world.add_object(PointMass(Vec2(800, 450), Vec2(0, 300), 100.0, RED));
  world.add_object(PointMass(Vec2(1000, 450), Vec2(0, -300), 100.0, BLUE));
  return world;
}

World star_planet() {
  World world;
  world.add_object(PointMass(Vec2(900, 450), Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(Vec2(500, 450), Vec2(0, 500), 1, BLUE));
  return world;
}
  

int main() {
  InitWindow(screen_width, screen_height, "phys");
  SetTargetFPS(FPS);

  World world = star_planet();

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(BLACK);
    world.draw();

    EndDrawing();

    world.step();
  }

  CloseWindow();
  
  return 0;
}
