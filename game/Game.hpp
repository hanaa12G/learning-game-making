#pragma once
#include <list>
#include <cassert>
#include <cstdio>
#include <vector>

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

  Player(glm::vec3 position = {0, 0, 0}, glm::vec3 direction = {0, 0, 0}) : GameObject(position), dir(direction) {}
  char const* Type() override { return "Player"; }
  char const* Texture() override { assert(false); }

  void Move(glm::vec3 d) {
    camera->Move(d);
    fprintf(stdout, "Position: %f, %f, %f\n", camera->location.x, camera->location.y, camera->location.z);
  }
  void Rotate(float yaw, float pitch) {
    camera->Rotate(yaw, pitch);
  }
};

struct Renderer {
  virtual ~Renderer() {}
  virtual void SetActiveCamera(Camera& camera) {}
  virtual void Draw(GameObject& obj) {}
  virtual void DrawBatch(std::vector<GameObject*>& obj) {}
};

struct Game {
  bool init = false;
  std::vector<GameObject*> objects;
  Player player;
};



extern "C" {

  void game_update(Game& game, Renderer& renderer);
}
