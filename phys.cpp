#include <raylib.h>
#include <vector>
#include <iostream>
#include <cmath>

const int FPS = 60;

class Vec2 {
public:
  double x, y;

  Vec2(double x, double y) : x{x}, y{y} {}

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

  Vec2 operator/(const double lambda) {
    return Vec2(x / lambda, y / lambda);
  }

  Vec2 operator*(const double lambda) {
    return Vec2(x * lambda, y * lambda);
  }

  void print() {
    std::cout << "(" << x << ", " << y << ")\n" << std::flush;
  }

  Vec2 discrete() {
    return *this / (double) FPS;
  }

  double norm() {
    return sqrt(x*x + y*y);
  }

  double dot(const Vec2 &rhs) {
    return x * rhs.x + y * rhs.y;
  }


  friend Vec2 operator*(const double lambda, const Vec2 &v);
};

const int screen_width = 1920, screen_height = 1080;
Vec2 ORIGIN(screen_width / 2, screen_height / 2);

Vec2 origin(float x, float y) {
  return Vec2(ORIGIN.x + x, ORIGIN.y + y);
}


Vec2 operator*(const double lambda, const Vec2 &v) {
  return Vec2(v.x * lambda, v.y * lambda);
}

class PointMass {
public:
  Vec2 pos, vel = Vec2(0, 0);
  double mass;
  int radius = 10;
  Color color = GREEN;

  PointMass(Vec2 pos, double mass) : pos{pos}, mass{mass} {}
  PointMass(Vec2 pos, double mass, Color color) : pos{pos}, mass{mass}, color{color} {}
  PointMass(Vec2 pos, Vec2 vel, double mass) : pos{pos}, vel{vel}, mass{mass} {}
  PointMass(Vec2 pos, Vec2 vel, double mass, Color color)
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


// return gravitational force acting on p due to q
Vec2 gravity(PointMass &p, PointMass &q) {
  double G = 1;
  double dist = (q.pos - p.pos).norm();
  Vec2 force = (G * p.mass * q.mass * (q.pos - p.pos)) / (dist * dist * dist);
  return force;
}


// return gravitational potential energy of p due to q
double grav_potential(PointMass &p, PointMass &q) {
  double G = 1e3;
  double dist = (q.pos - p.pos).norm();
  return (-G * p.mass * q.mass) / dist;
}


class World {
  //Vec2 gravity = Vec2(0, 200);
  Vec2 world_gravity = Vec2(0, 100);
  std::vector<PointMass> objects;

public:
  double energy; // total mechanical energy

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
      //auto p = &new_obj[i];
      auto p = &objects[i];

      for (int j = 0; j < objects.size(); j++) {
        if (i == j) continue;
        auto q = &objects[j];
        p->vel += (gravity(*p, *q) / p->mass).discrete();
      }

      p->pos += p->vel.discrete();
      //p->vel = collide(p->pos, p->vel);
    }

    //objects = new_obj;
  }

  void get_energy() {
    double k = 0, u = 0;

    for (auto p: objects) {
      k += 0.5 * p.mass * p.vel.norm() * p.vel.norm();
    }

    for (int i = 0; i < objects.size(); i++) {
      auto p = &objects[i];
      for (int j = 0; j < objects.size(); j++) {
        if (i == j) continue;
        auto q = &objects[j];
        u += (-gravity(*p, *q)).dot(q->pos - p->pos);
        //u += grav_potential(*p, *q);
      }
    }

    std::cout << "kinetic: " << k << ", potential: " << u
              << ", total: " << k + u << '\n';
  }
};


World two_body() {
  World world;
  world.add_object(PointMass(origin(-200, 0), Vec2(0, 0), 1e7, RED));
  world.add_object(PointMass(origin(200, 0), Vec2(0, 0), 1e7, BLUE));
  return world;
}

World three_body() {
  World world;
  world.add_object(PointMass(Vec2(800, 550), Vec2(-100, 150), 1e4, RED));
  world.add_object(PointMass(Vec2(1000, 550), Vec2(100, -150), 1e4, BLUE));
  world.add_object(PointMass(Vec2(900, 450), Vec2(-200, 100), 1e4, GREEN));
  return world;
}



World star_planet() {
  World world;
  world.add_object(PointMass(Vec2(900, 450), Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(Vec2(500, 450), Vec2(0, 400), 1, BLUE));
  return world;
}

World star_two_planet() {
  World world;
  world.add_object(PointMass(Vec2(900, 450), Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(Vec2(500, 450), Vec2(0, 400), 1, BLUE));
  world.add_object(PointMass(Vec2(500, 350), Vec2(0, 400), 1, GREEN));
  return world;
}
  
World star_planet_moon() {
  World world;
  PointMass sun = PointMass(Vec2(1000, 540), Vec2(0, 0), 1e8, RED);
  PointMass planet = PointMass(Vec2(400, 540), Vec2(0, 400), 1e5, BLUE);
  PointMass moon = PointMass(Vec2(380, 540), Vec2(0, 450), 1, GREEN);
  sun.radius = 20;
  planet.radius = 10;
  moon.radius = 5;
  world.add_object(sun);
  world.add_object(planet);
  world.add_object(moon);
  return world;
}


int main() {
  InitWindow(screen_width, screen_height, "phys");
  SetTargetFPS(FPS);

  World world = two_body();

  while (!WindowShouldClose()) {
    world.get_energy();
    BeginDrawing();

    ClearBackground(BLACK);
    world.draw();

    EndDrawing();

    world.step();
  }

  CloseWindow();
  
  return 0;
}
