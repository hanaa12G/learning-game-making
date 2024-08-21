#include "Collision.hpp"
#include "Objects.hpp"

#include "Debug.cpp"
#include <array>
#include <cmath>
#include <iostream>

bool CollisionDetectionStrategy2DUseAABB::is_collide(CollisionBody2D const &a,
                                                     CollisionBody2D const &b) {
  static auto is_collide_bvb = [](auto const &a, auto const &b) -> bool {
    auto const &box_a = std::get<CollisionShape2DBox>(a.m_shape);
    auto const &box_b = std::get<CollisionShape2DBox>(b.m_shape);

    auto topleft_a = box_a.top_left();
    auto bottomright_a = box_a.bottom_right();
    auto topleft_b = box_b.top_left();
    auto bottomright_b = box_b.bottom_right();

    bool collide_if =
        (topleft_a.x < bottomright_b.x and bottomright_a.x > topleft_b.x) and
        (topleft_a.y > bottomright_b.y and bottomright_a.y < topleft_b.y);

    return not collide_if;
  };

  static auto is_collide_bvs = [](auto const &a, auto const &b) -> bool {
    return false;
  };

  static auto is_collide_bvc = [](auto const &a, auto const &b) -> bool {
    return false;
  };

  static auto is_collide_svb = [](auto const &a, auto const &b) -> bool {
    return is_collide_bvs(b, a);
  };

  static auto is_collide_svs = [](auto const &a, auto const &b) -> bool {
    CollisionShape2DSphere const &sphere_a =
        std::get<CollisionShape2DSphere>(a.m_shape);
    CollisionShape2DSphere const &sphere_b =
        std::get<CollisionShape2DSphere>(b.m_shape);

    float sphere_radius_a = sphere_a.m_radius;
    float sphere_radius_b = sphere_b.m_radius;
    float larger_radius = std::max(sphere_radius_a, sphere_radius_b);

    DistanceDetectorStrategy2DUsePythagorean distance_detector;
    float distance = distance_detector.distance_between(a, b);
    return distance < larger_radius;
  };

  static auto is_collide_svc = [](auto const &a, auto const &b) -> bool {
    return false;
  };

  static auto is_collide_cvb = [](auto const &a, auto const &b) -> bool {
    return is_collide_bvc(b, a);
  };

  static auto is_collide_cvs = [](auto const &a, auto const &b) -> bool {
    return is_collide_svc(b, a);
  };

  static auto is_collide_cvc = [](auto const &a, auto const &b) -> bool {
    return false;
  };

  using Func = bool (*)(CollisionBody2D const &, CollisionBody2D const &);

  std::array<Func, 3 * 3> funcs = {
      is_collide_bvb, is_collide_bvs, is_collide_bvc,
      is_collide_svb, is_collide_svs, is_collide_svc,
      is_collide_cvb, is_collide_cvs, is_collide_cvc};

  std::cout << "Collision: [DEBUG] a.body: " << a.m_position  << ", b.body"
  << b.m_position << std::endl;
  bool collided = funcs.at(a.m_shape.index() * b.m_shape.index())(a, b);
  if (collided)
    std::cout << "Collision: [INFO] Collision found" << std::endl;
  return collided;
}

void CollisionDetectionStrategy2DUseAABB::resolve_collision(
    CollisionBody2D &a, CollisionBody2D &b) {}

float DistanceDetectorStrategy2DUsePythagorean::distance_between(
    CollisionBody2D const &a, CollisionBody2D const &b) {
  float diff_x = a.m_position.x - b.m_position.x;
  float diff_y = a.m_position.y - b.m_position.y;

  float distance = std::sqrt(std::pow(diff_x, 2) + std::pow(diff_y, 2));

  std::cout << "DistanceDetectorStrategy2DUsePythagorean: [DEBUG] " << distance
            << std::endl;

  return distance;
}
