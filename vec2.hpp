#ifndef VEC2_HPP
#define VEC2_HPP

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

  double norm() {
    return sqrt(x*x + y*y);
  }

  double dot(const Vec2 &rhs) {
    return x * rhs.x + y * rhs.y;
  }


  friend Vec2 operator*(const double lambda, const Vec2 &v);
};

Vec2 operator*(const double lambda, const Vec2 &v) {
  return Vec2(v.x * lambda, v.y * lambda);
}

#endif
