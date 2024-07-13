#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include <vector>

#include "vec2.hpp"
#include "world.hpp"

class VerletIntegrator {
  void compute_acc() {
    aprev = acur;
    acur.clear();
    for (auto f: world.binary_forces) {
      for (int i = 0; i < world.objects.size(); i++) {
        auto &p = world.objects[i];
        Vec2 acc(0, 0);
        for (int j = 0; j < world.objects.size(); j++) {
          if (i == j) continue;
          auto &q = world.objects[j];
          acc += f->force(p, q) / p.mass;
        }
        acur.push_back(acc);
      }
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
  double dt;
  
  VerletIntegrator(World &world, double dt) : world{world}, dt{dt} {
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

#endif
