#pragma once
#include <glm/geometric.hpp>
#include <list>
#include <cassert>
#include <cstdio>
#include <vector>
#include <chrono>

namespace chrono = std::chrono;

#include "Camera.hpp"


struct GameObject {
  glm::vec3 pos;
  GameObject(glm::vec3 p) : pos (p) {}

  virtual ~GameObject() {}
  virtual char const* Type() = 0;
  virtual char const* Texture() = 0;
};

struct Dirt : public GameObject {
  using GameObject::GameObject;
  static char const* texture_path;

  char const* Type() override { return "Dirt"; }
  char const* Texture() override { return texture_path; }
};

struct Rock : public GameObject {

  using GameObject::GameObject;
  static char const* texture_path;

  char const* Type() override { return "Rock"; }
  char const* Texture() override { return texture_path; }
};

struct Player : public GameObject {
  glm::vec3 dir;
  Camera* camera;
  bool should_move;
  glm::vec3 velocity;
  float speed = 0.0f;
  float acceleration = 10.0f;
  chrono::time_point<chrono::high_resolution_clock> velocity_set_timestamp;

  Player(glm::vec3 position = {0, 0, 0}, glm::vec3 direction = {0, 0, 0}) : GameObject(position), dir(direction), should_move (true) {}
  char const* Type() override { return "Player"; }
  char const* Texture() override { assert(false); }

  void Move(glm::vec3 d) {
    if (!should_move) return;
    camera->Move(d);
    pos += d;
    fprintf(stdout, "Position: %f, %f, %f\n", camera->location.x, camera->location.y, camera->location.z);
  }
  void Rotate(float yaw, float pitch) {
    camera->Rotate(yaw, pitch);
  }

  void UpdateVelocity() {
    if (glm::length(velocity) != 0) {
      velocity_set_timestamp = chrono::high_resolution_clock::now();
    }
  }
};

struct Renderer {
  virtual ~Renderer() {}
  virtual void SetActiveCamera(Camera& camera) {}
  virtual void Draw(GameObject& obj) {}
  virtual void DrawBatch(std::vector<GameObject*>& obj) {}
};

struct Grid {
  using Cell = GameObject*;

  long dim;

  Grid(long d = 0) {
    dim = d;
    long size = dim * dim * dim;
    data.resize(size, nullptr);
  }

  Cell& at(int x, int y, int z) {
    x += dim / 2;
    y += dim / 2;
    z += dim / 2;
    return data.at(x * dim * dim + y * dim + z);
  }

  std::vector<Cell> data;
};


struct InputButton {
  bool is_down;
  int transition_count;
  chrono::time_point<chrono::high_resolution_clock> down_timestamp;

  float duration() {
    auto now = chrono::high_resolution_clock::now();
    auto dur = chrono::duration_cast<chrono::duration<float>>(now - down_timestamp);
    return dur.count();
  }
};
struct InputMouse {
  enum {
    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonMiddle,
    MouseButtonCount,
  };

  int x;
  int y;
  InputButton mouse_buttons[MouseButtonCount];
};

struct GameInput {
  enum {
    ButtonUp,
    ButtonDown,
    ButtonLeft,
    ButtonRight,
    ButtonFront,
    ButtonBack,
    ButtonCount
  };
  InputButton buttons[ButtonCount];
  InputMouse mouse;
};

struct Game {
  bool init = false;
  std::vector<GameObject*> objects;
  Player player;
  Grid grid;
};



extern "C" {

  void game_update(Game& game, Renderer& renderer, GameInput& input, float elapsed_time);
}
