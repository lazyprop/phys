#include <raylib.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "vec2.hpp"
#include "integrator.hpp"
#include "world.hpp"

const int FPS = 60;

const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
Vec2 ORIGIN(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

Vec2 origin(float x, float y) {
  return Vec2(ORIGIN.x + x, ORIGIN.y + y);
}

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


World two_body() {
  World world;
  world.add_object(PointMass(origin(-200, 0), Vec2(0, 50), 1e7, RED));
  world.add_object(PointMass(origin(200, 0), Vec2(0, -50), 1e7, BLUE));
  world.add_binary_force((BinaryForce*) new Gravity(1));
  return world;
}

World three_body() {
  World world;
  world.add_object(PointMass(origin(100, 0), Vec2(0, 100), 1e7, RED));
  world.add_object(PointMass(origin(-50, 50 * sqrt(3)),
                             Vec2(-sqrt(3) * 50, -50), 1e7, BLUE));
  world.add_object(PointMass(origin(-50, -50 * sqrt(3)),
                             Vec2(sqrt(3) * 50, -50), 1e7, GREEN));
  world.add_binary_force((BinaryForce*) new Gravity(1));
  return world;
}



World star_planet() {
  World world;
  world.add_object(PointMass(ORIGIN, Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(origin(-100, 0), Vec2(0, 100), 5, BLUE));
  world.add_binary_force((BinaryForce*) new Gravity(1));
  return world;
}

World star_two_planet() {
  World world;
  world.add_object(PointMass(Vec2(900, 450), Vec2(0, 0), 1e5, RED));
  world.add_object(PointMass(Vec2(500, 450), Vec2(0, 400), 1, BLUE));
  world.add_object(PointMass(Vec2(500, 350), Vec2(0, 400), 1, GREEN));
  world.add_binary_force((BinaryForce*) new Gravity(1));
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
  world.add_binary_force((BinaryForce*) new Gravity(1));
  return world;
}


int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "phys");
  SetTargetFPS(FPS);

  World world = three_body();
  VerletIntegrator integrator(world, 1.0 / FPS);

  while (!WindowShouldClose()) {
    world.compute_energy();
    BeginDrawing();

    ClearBackground(BLACK);
    world.draw();

    EndDrawing();

    integrator.step();
  }

  CloseWindow();
  
  return 0;
}
