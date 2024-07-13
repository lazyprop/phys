#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include "vec2.hpp"

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


class UnaryForce {
  virtual Vec2 force(PointMass &p) const = 0;
  virtual double potential_energy(PointMass &p) const = 0;
};

class BinaryForce {
public:
  virtual Vec2 force(PointMass &p, PointMass &q) const = 0;
  virtual double potential_energy(PointMass &p, PointMass &q) const = 0;
};
  

class Gravity : public BinaryForce {
  double G = 1;
public:
  Gravity(double G) : G{G} {}
  
  virtual Vec2 force(PointMass &p, PointMass &q) const {
    double dist = (q.pos - p.pos).norm();
    if (dist < 1e-12) return Vec2(0, 0);
    return (G * p.mass * q.mass * (q.pos - p.pos)) / (dist * dist * dist);
  }

  virtual double potential_energy(PointMass &p, PointMass &q) const {
    double dist = (q.pos - p.pos).norm();
    if (dist < 1e-12) return 0;
    return (-G * p.mass * q.mass) / dist;
  }
};


class World {
  //Vec2 gravity = Vec2(0, 200);
  Vec2 world_gravity = Vec2(0, 100);

public:
  double energy; // total mechanical energy
  std::vector<PointMass> objects;
  std::vector<BinaryForce*> binary_forces;

  World() : objects{}, binary_forces{} {}

  void add_object(PointMass p) {
    objects.push_back(p);
  }

  void add_binary_force(BinaryForce* f) {
    binary_forces.push_back(f);
  }

  void draw() {
    for (auto o : objects) {
      o.draw();
    }
  }

  void compute_energy() {
    double k = 0, u = 0;
    for (auto p: objects) {
      k += 0.5 * p.mass * p.vel.norm() * p.vel.norm();
    }
    for (auto f: binary_forces) {
      for (int i = 0; i < objects.size(); i++) {
        auto &p = objects[i];
        for (int j = 0; j < objects.size(); j++) {
          if (i == j) continue;
          auto &q = objects[j];
          u += 0.5 * f->potential_energy(p, q);
        }
      }
    }
    energy = k + u;
  }
};

#endif
