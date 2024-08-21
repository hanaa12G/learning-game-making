#pragma once
#include <glm/geometric.hpp>
#include <list>
#include <cassert>
#include <cstdio>
#include <vector>
#include <chrono>
#include <memory>
#include <string>

namespace chrono = std::chrono;

#include "Camera.hpp"
#include "Objects.hpp"

struct GameObject {
  glm::vec3 pos;
  std::string m_id;
  GameObject(glm::vec3 p, char const* id = "") : pos (p) , m_id {id} {}

  virtual ~GameObject() {}
  virtual std::string Type() = 0;
  virtual char const* Texture() = 0;
  std::string Id() { return m_id; }
};

struct Dot : public GameObject {
  using GameObject::GameObject;

  std::string Type() override { return "Dot"; };
  char const* Texture() override { return nullptr; }
};

struct Line : public GameObject {
  using GameObject::GameObject;

  glm::vec3 end;

  Line(glm::vec3 start, glm::vec3 e, char const* id = "") : GameObject(start, id), end (e) {}

  glm::vec3 Ray() {
    return end - pos;
  }

  std::string Type() override { return "Line"; };
  char const* Texture() override { return nullptr; }
};
struct Rectangle : public GameObject {
  float width;
  float height;
  Rectangle(glm::vec3 pos, float w, float h, char const* id = "") : GameObject (pos, id),  width (w), height (h) {}
  Rectangle(glm::vec3 topleft, glm::vec3 bottomright, char const* id = "") : GameObject ( topleft + (bottomright - topleft) / 2.0f , id), width (bottomright.x - topleft.x), height (topleft.y - bottomright.y) {}

  glm::vec3 TopLeft() { return glm::vec3 { pos.x - width / 2.0f, pos.y + height / 2.0f, 0.0f }; }
  glm::vec3 BottomRight() { return  glm::vec3 { pos.x + width / 2.0f, pos.y - height / 2.0f, 0.0f }; }

  std::string Type() override { return "Rectangle"; };
  char const* Texture() override { return nullptr; }
};

struct Dirt : public GameObject {
  using GameObject::GameObject;
  static char const* texture_path;

  std::string Type() override { return "Dirt"; }
  char const* Texture() override { return texture_path; }
};

struct Rock : public GameObject {

  using GameObject::GameObject;
  static char const* texture_path;

  std::string Type() override { return "Rock"; }
  char const* Texture() override { return texture_path; }
};

struct Player : public GameObject {
  glm::vec3 dir;
  Camera* camera;
  glm::vec3 velocity;
  float acceleration = 10.0f;
  chrono::time_point<chrono::high_resolution_clock> velocity_set_timestamp;

  Player(glm::vec3 position = {0, 0, 0}, glm::vec3 direction = {0, 0, 0}) : GameObject(position), dir(direction){}
  std::string Type() override { return "Player"; }
  char const* Texture() override { assert(false); }

  void SetCamera(Camera* c) {
    assert(c);
    camera = c;
    camera->direction = dir;
    camera->location = pos + glm::vec3(0.0, 1.0, 0.0);
    camera->world_up = glm::vec3(0.0, 1.0f, 0.0f);
    camera->UpdateVector();
  }
  void Move(glm::vec3 d) {
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

struct Scene {
  std::vector<GameObject*> objects;
  Player player;
  Grid grid;

  void Render();
};

struct Game {
  bool init = false;
  std::vector<std::shared_ptr<Scene>> scenes;
  unsigned int scene_index;

  unsigned int viewport_width;
  unsigned int viewport_height;

  void SwitchScene() {
    scene_index += 1;
    if (scene_index == scenes.size()) scene_index = 0;
  }

  Scene& CurrentScene() {
    if (scene_index <= scenes.size()) {
      return *scenes[scene_index].get();
    }
    assert(false);
  }
};

struct Scene2D {
  std::vector<GameObject2D> m_objects;

  GameObject2D& get_player(std::string name = "Player");
};

struct VisualBody2D;

struct Renderer2D {
  virtual ~Renderer2D() {}

  virtual void draw(VisualBody2D const&) = 0;
  virtual void set_screen_size(unsigned, unsigned) = 0;
};

struct Game2D {
  std::vector<Scene2D> m_scenes;

  std::unique_ptr<Renderer2D> m_renderer;
  

};

bool collision_check(GameObject& a, GameObject& b);
void collision_fix(GameObject& a, GameObject const& b);



extern "C" {

  void game_update(Game& game, Renderer& renderer, GameInput& input, float elapsed_time);
  void game2d_init(Game2D* game);
  void game2d_update(Game2D* game, GameInput* input, float time_elapsed);
}
