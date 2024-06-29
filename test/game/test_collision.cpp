#include "game/Game.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("collision check 1") {
  Dirt a({0, 1, 0});
  Dirt b({0, 0, 0});
  REQUIRE(collision_check(a, b) == false);
}

TEST_CASE("collision fixup 1") {
  Dirt a({0, 1, 0});
  Dirt b({0, 0.8, 0});
  // fixup_location(b, a);
  REQUIRE(b.pos == glm::vec3(0, 1.0, 0));
}
