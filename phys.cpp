#include <raylib.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

const int FPS = 60;
const double dt = 1.0 / FPS;
const double G = 1;

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
    std::cout << std::fixed;
    std::cout << std::setprecision(4) << "(" << x << ", " << y << ")\n" << std::flush;
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

const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
Vec2 ORIGIN(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

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
  if (pos.x > SCREEN_WIDTH or pos.x < 0) {
    return Vec2(-vel.x, vel.y);
  }
  if (pos.y > SCREEN_HEIGHT or pos.y < 0) {
    return Vec2(vel.x, -vel.y);
  }
  return vel;
}


// return gravitational force acting on p due to q
Vec2 gravity(PointMass &p, PointMass &q) {
  double dist = (q.pos - p.pos).norm();
  Vec2 force = (G * p.mass * q.mass * (q.pos - p.pos)) / (dist * dist * dist);
  return force;
}


// return gravitational potential energy of p due to q
double grav_potential(PointMass &p, PointMass &q) {
  double dist = (q.pos - p.pos).norm();
  return (-G * p.mass * q.mass) / dist;
}


class World {
  //Vec2 gravity = Vec2(0, 200);
  Vec2 world_gravity = Vec2(0, 100);

public:
  double energy; // total mechanical energy
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
    std::vector<PointMass> newobj(objects);
    for (auto &p: newobj) {
      for (auto &q: objects) {
        if ((p.pos - q.pos).norm() < 0.1) continue;
        p.vel += (gravity(p, q) / p.mass) * dt;
      }
      p.pos += p.vel * dt;
    }
    objects = newobj;
  }

  void get_energy() {
    double k = 0, u = 0;
    for (auto p: objects) {
      k += 0.5 * p.mass * p.vel.norm() * p.vel.norm();
    }
    for (auto &p: objects) {
      for (auto &q: objects) {
        if ((p.pos - q.pos).norm() < 0.1) continue;
        u += 0.5 * grav_potential(p, q);
      }
    }

    std::cout << std::fixed << std::setprecision(5) << k + u << '\n';
  }
};

class VerletIntegrator {
  void compute_acc() {
    aprev = acur;
    acur.clear();
    for (auto &p: world.objects) {
      Vec2 acc(0, 0);
      for (auto &q: world.objects) {
        if ((q.pos - p.pos).norm() < 0.1) continue;
        acc += gravity(p, q) / p.mass;
      }
      acur.push_back(acc);
    }
  }

  void compute_vel() {
    std::vector<Vec2> vnew;
    for (int i = 0; i < vcur.size(); i++) {
      vnew.push_back(vcur[i] + 0.5 * (aprev[i] + acur[i]) * dt);
    }
    vprev = vcur;
    vcur = vnew;
  }
    

  // set world to 'cur' state
  void update_world() {
    for (int i = 0; i < xcur.size(); i++) {
      world.objects[i].pos = xcur[i];
      world.objects[i].vel = vcur[i];
    }
  }

public:
  World &world;
  std::vector<Vec2> xprev, vprev, xcur, vcur, aprev, acur;
  
  VerletIntegrator(World &world) : world{world} {
    for (auto &p: world.objects) {
      xprev.push_back(p.pos);
      vprev.push_back(p.vel);
    }

    compute_acc();

    for (int i = 0; i < xprev.size(); i++) {
      xcur.push_back(xprev[i] + vprev[i] * dt + 0.5 * acur[i]* dt * dt);
    }

    vcur = vprev;
    update_world();
    compute_acc();
    compute_vel();
    update_world();
  }


  void step() {
    std::vector<Vec2> newx;
    for (int i = 0; i < xprev.size(); i++) {
      newx.push_back(2 * xcur[i] - xprev[i] + acur[i] * dt * dt);
    }

    xprev = xcur;
    xcur = newx;
    update_world();
    compute_acc();
    compute_vel();
    update_world();
  }
};

World two_body() {
  World world;
  world.add_object(PointMass(origin(-200, 0), Vec2(0, 50), 1e7, RED));
  world.add_object(PointMass(origin(200, 0), Vec2(0, -50), 1e7, BLUE));
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
  world.add_object(PointMass(ORIGIN, Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(origin(-100, 0), Vec2(0, 100), 5, BLUE));
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
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "phys");
  SetTargetFPS(FPS);

  World world = two_body();
  VerletIntegrator integrator(world);

  while (!WindowShouldClose()) {
    world.get_energy();
    BeginDrawing();

    ClearBackground(BLACK);
    world.draw();

    EndDrawing();

    integrator.step();
  }

  CloseWindow();
  
  return 0;
}
