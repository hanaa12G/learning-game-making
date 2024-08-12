#include "Game.hpp"
#include <chrono>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <map>
#include "Collision.hpp"
#include "Debug.cpp"
#include <algorithm>
#include <tuple>

char const* Dirt::texture_path = "/home/wsluser/Workspace/game/runtime/coarse_dirt.png";
char const* Rock::texture_path = "/home/wsluser/Workspace/game/runtime/cobblestone.png";


bool collision_check(GameObject& a, GameObject& b) {
  // ------------|
  // ------|---------------
  bool y = (a.pos.x + 1.0 <= b.pos.x  || a.pos.x >= b.pos.x + 1.0) ||
           (a.pos.y + 1.0 <= b.pos.y  || a.pos.y >= b.pos.y + 1.0) ||
           (a.pos.z + 1.0 <= b.pos.z  || a.pos.z >= b.pos.z + 1.0);
  return !y;
}

struct CollisionBox {
  glm::vec3 a;
  glm::vec3 b;
};

void collision_fix(CollisionBox& obj_1, CollisionBox const& obj_2) {
  auto obj_2_diag = obj_2.b - obj_2.a;
  auto obj_2_center = obj_2.a + (obj_2_diag / 2.0f);
  
}

void fixup_location(Player& player, GameObject& o) {
  float o_x0 = o.pos.x;
  float o_x1 = o.pos.x + 1.0;
  float p_x0 = player.pos.x;
  float p_x1 = player.pos.x + 1.0;

  float dx = 0.0f;
  if (o_x0 >= p_x0) dx = o_x0 - p_x1;
  else if (o_x1 <= p_x1) dx = o_x1 - p_x0;


  float o_y0 = o.pos.y;
  float o_y1 = o.pos.y + 1.0;
  float p_y0 = player.pos.y;
  float p_y1 = player.pos.y + 1.0;

  float dy = 0.0f;
  if (o_y0 >= p_y0) dy = o_y0 - p_y1;
  else if (o_y1 <= p_y1) dy = o_y1 - p_y0;


  float o_z0 = o.pos.z;
  float o_z1 = o.pos.z + 1.0;
  float p_z0 = player.pos.z;
  float p_z1 = player.pos.z + 1.0f;

  float dz = 0.0f;
  if (o_z0 >= p_z0) dz = o_z0 - p_z1;
  else if (o_z1 <= p_z1) dz = o_z1 - p_z0;

  player.Move(glm::vec3(dx, dy, dz));
}

void update_player_from_input(Player& player, GameInput& input) {
  float running_duration = 1.0f;

  if (input.buttons[GameInput::ButtonUp].is_down) {
    if (input.buttons[GameInput::ButtonUp].duration() >= running_duration) {
      player.velocity = glm::normalize(player.camera->direction) * 10.0f;
    }
    else {
      player.velocity = glm::normalize(player.camera->direction) * 1.0f;
    }
    player.acceleration = 0.0f;
  }
  else if (!input.buttons[GameInput::ButtonUp].is_down) {
    if (glm::length(player.velocity) >= 0) {
      player.acceleration = 10.0f;
      player.UpdateVelocity();
    }
  }
}

std::tuple<bool, glm::vec3, glm::vec3> check_collision(Line* line, Rectangle* rect) {
  assert(line);
  assert(rect);

  auto ray_direction = glm::normalize(line->Ray());
  auto cos_theta = ray_direction.x;
  auto sin_theta = ray_direction.y;

  auto origin = line->pos;
  auto topleft = rect->TopLeft();
  auto bottomright = rect->BottomRight();

  auto dx = topleft.x - origin.x;
  auto dy = bottomright.y - origin.y;

  auto w = rect->width;
  auto h = rect->height;

  auto t1x = dx / cos_theta;
  auto t2x = (dx + w) / cos_theta;
  auto t1y = dy / sin_theta;
  auto t2y = (dy + h) / sin_theta;
  
  // std::cout << "t1x: " << t1x << ", t2x: " << t2x << ", t1y: " << t1y << ", t2y: " << t2y << std::endl;

  struct Item {
    float value;
    int x; // 0: not x, 1: 1x, 2: 2x
    int y; // 0: not y, 1: 1y, 2: 2y
  };

  std::vector<Item> array = {
    { t1x, 1, 0 },
    { t2x, 2, 0 },
    { t1y, 0, 1 },
    { t2y, 0, 2 },
  };

  std::sort(array.begin(), array.end(), [] (auto const& a, auto const& b) {
    return a.value < b.value;
  });

  for (auto array_element : array) {
    // std::cout << "array_element.x: " << array_element.x << ", array_element.y: " << array_element.y 
    //   << ", array_element.value: " << array_element.value << std::endl;
  }

  if (array[0].x * array[1].x > 0 || array[0].y * array[1].y > 0) {
    return std::make_tuple(false, glm::vec3(), glm::vec3());
  }


  glm::vec3 point (0.0f, 0.0f, 0.0f);
  auto item = array[1];
  auto value = item.value;

  
  // if the ray length is not enough, it's not going to collide with
  // the rectangle
  if (item.value >= glm::length(line->Ray())) {
    return std::make_tuple(false, glm::vec3(), glm::vec3());
  }


  glm::vec3 collision_point = origin + glm::vec3(value * cos_theta, value * sin_theta, 0.0f);
  glm::vec3 normal {};


  const float very_small_f = 0.0001f;
  if (std::abs(collision_point.x - topleft.x) <= very_small_f) {
    normal = glm::vec3(-1, 0, 0);
  }
  else if (std::abs(collision_point.x - bottomright.x) <= very_small_f) {
    normal = glm::vec3(1, 0, 0);
  }
  else if (std::abs(collision_point.y - topleft.y) <= very_small_f) {
    normal = glm::vec3(0, 1, 0);
  }
  else if (std::abs(collision_point.y - bottomright.y) <= very_small_f) {
    normal = glm::vec3(0, -1, 0);
  }
  else assert(false);

  return std::make_tuple(true, collision_point, normal);
}

void game_update_input(Game& game, GameInput& input) {
  if (game.scene_index != 1) return;
  // std::cout << input.mouse.x << " " << input.mouse.y << std::endl;

  auto mouse_pos = glm::vec3 {
    2.0f * ((float) (input.mouse.x) / (float) (game.viewport_width) - 0.5),
      -2.0f * ((float) (input.mouse.y) / (float) (game.viewport_height) - 0.5),
      0.0f
  };

  auto& objects = game.CurrentScene().objects;


  objects.erase(std::remove_if(objects.begin(), objects.end(),
  [] (auto obj)
  {
    if (std::string("Dot") == obj->Type() && std::string("Origin") != obj->Id() && std::string("Bullet") != obj->Id()) {
      delete obj;
      return true;
    }
    return false;
  }), objects.end());

  if (input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down) {
    std::cout << "Mouse button down" << std::endl;

    objects.erase(std::remove_if(objects.begin(), objects.end(),
    [] (auto obj)
    {
      if (std::string("Dot") == obj->Type() && std::string("Origin") == obj->Id()) {
        delete obj;
        return true;
      }
      return false;
    }), objects.end());


    auto dot = new Dot(mouse_pos, "Origin");

    objects.push_back(dot);
    std::cout << "Set new point to " << objects.back()->pos << std::endl;
  }
  else {

    objects.erase(std::remove_if(objects.begin(), objects.end(),
    [] (auto obj)
    {
      if (std::string("Line") == obj->Type()) {
        delete obj;
        return true;
      }
      return false;
    }), objects.end());

    auto dot_iterator = std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        if (std::string("Dot") == obj->Type() && std::string("Origin") == obj->Id()) {
        return true;
        }
        return false;
        });
    if (dot_iterator != objects.end()) {
      auto dot = *dot_iterator;
      auto line = new Line(dot->pos, mouse_pos);
      objects.push_back(line);
    }
  }

  if (input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down) {
    std::cout << "Mouse right button down" << std::endl;
    auto line_iterator =  std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        return std::string("Line") == obj->Type();
        });
    if (line_iterator == objects.end()) goto no_firing;

    objects.push_back(new Dot(mouse_pos, "Bullet"));
 
no_firing:
;
  }



  auto rect_iterator = std::find_if(objects.begin(), objects.end(), [] (auto obj) {
    return std::string("Rectangle") == obj->Type();
  });
  if (rect_iterator == objects.end()) {
    objects.push_back(new Rectangle(
       {0.0f, 0.0f, 0.0f},
       0.4f, 0.4f
    ));
  }

  {
    auto line_iterator = std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        return std::string("Line") == obj->Type();
        });
    auto bullet_iterator = std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        return std::string("Dot") == obj->Type() && std::string("Bullet") == obj->Id();
        });

    if (line_iterator != objects.end() && bullet_iterator != objects.end()) {
    }
  }


  {

    auto rect_iterator = std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        return std::string("Rectangle") == obj->Type();
        });

    auto line_iterator =  std::find_if(objects.begin(), objects.end(), [] (auto obj) {
        return std::string("Line") == obj->Type();
        });

    if (rect_iterator != objects.end() && line_iterator != objects.end()) {
      auto res = check_collision(dynamic_cast<Line*>(*line_iterator), dynamic_cast<Rectangle*>(*rect_iterator));

      /// if we have collision, visualize it
      if (std::get<0>(res)) { 
        auto point = std::get<1>(res);
        objects.push_back(new Dot(point));

        auto collision_normal = new Line(point, point + std::get<2>(res));
        objects.push_back(collision_normal);
      }
    }
  }

}

std::vector<GameObject2D*> find_close_objects_related_to(GameObject2D const& the, std::vector<GameObject2D> objects, float distance, DistanceDetectorStrategy2D& distance_detector);


extern "C" {
  void game_update(Game& game, Renderer& renderer, GameInput& input, float elapsed_time) {
    if (!game.init) {
      {
        // Init main scene
        auto scene_normal = std::make_shared<Scene>();
        scene_normal->grid = Grid(100);
        for (int i = -50; i < 50; ++i) {
          for (int j = -50; j < 50; ++j) {
            int x = i;
            int z = j;

            scene_normal->grid.at(x, 0, z) = new Dirt({x, 0, z});
          }
        }

        scene_normal->player = Player({2, 10, 2});
        scene_normal->player.SetCamera(new Camera());
\
        scene_normal->player.velocity = glm::vec3(0);
        scene_normal->player.acceleration = 0.0f;
        scene_normal->player.velocity_set_timestamp = chrono::high_resolution_clock::now();

        game.scenes.push_back(scene_normal);
      }
      {
        // init test scene
        auto test_scene = std::make_shared<Scene>();
        test_scene->player = Player({0, 0, 1});
        test_scene->player.SetCamera(new Camera());
        
        test_scene->player.velocity = glm::vec3(0);
        test_scene->player.acceleration = 0.0f;
        test_scene->player.velocity_set_timestamp = chrono::high_resolution_clock::now();

        game.scenes.push_back(test_scene);
      }
      game.scene_index = 0;
      game.init = true;
    }

    if (input.buttons[GameInput::ButtonFront].is_down) {
      game.SwitchScene();
      std::cout << "Switched to scene: " << game.scene_index << std::endl;;
    }

    if (game.scene_index == 0) {
      update_player_from_input(game.CurrentScene().player, input);
      if (glm::length(game.CurrentScene().player.velocity) > 0) {
        auto time = elapsed_time;
        auto accelerator = -game.CurrentScene().player.acceleration* glm::normalize(game.CurrentScene().player.velocity);
        auto new_velocity = game.CurrentScene().player.velocity + accelerator * time;
        std::cout << new_velocity.x << " " << new_velocity.y << " " << new_velocity.z << std::endl;
        auto movement = game.CurrentScene().player.velocity * time;
        game.CurrentScene().player.Move(movement);

        auto new_velocity_normalized = glm::normalize(new_velocity);
        auto old_velocity_normalized = glm::normalize(game.CurrentScene().player.velocity);

        if (glm::dot(new_velocity_normalized, old_velocity_normalized) == -1) {
          game.CurrentScene().player.velocity = glm::vec3(0);
        } else {
          game.CurrentScene().player.velocity = new_velocity;
        }
      }

      for (auto object: game.CurrentScene().grid.data) {
        if (!object) continue;
        auto dir = game.CurrentScene().player.pos - object->pos;
        auto distance = glm::length(dir);
        if (distance > 5) {
          continue;
        }

        if (collision_check(game.CurrentScene().player, *object)) {
          std::cout << "Collision: player = " << game.CurrentScene().player.pos << ", object: " << object->pos << std::endl;
          fixup_location(game.CurrentScene().player, *object);
          std::cout << "Fixup: " << game.CurrentScene().player.pos << std::endl;
        }
      }

      std::map<std::string, std::vector<GameObject*>> batches;
      renderer.SetActiveCamera(*game.CurrentScene().player.camera);
      for (auto object: game.CurrentScene().grid.data) {
        if (!object) continue;
        batches[object->Type()].push_back(object);

      }

      for (auto& batch : batches) {
        renderer.DrawBatch(batch.second);
      }
    
    }

    else if (game.scene_index == 1) {
      game_update_input(game, input);
      renderer.SetActiveCamera(*game.CurrentScene().player.camera);
      for (auto obj: game.CurrentScene().objects) {
        renderer.Draw(*obj);
      }
    }

  }

  void game2d_init(Game2D* game) {
    assert(game);

    game->m_scenes.push_back(Scene2D());

    Scene2D& scene = game->m_scenes.front();

    GameObject2D player;
    player.load_inputs([] (GameObject2D& player, GameInput const& inputs) {
      glm::vec2 input_vec {};
      if (inputs.buttons[GameInput::ButtonLeft].is_down) input_vec.x = -1.0f;
      if (inputs.buttons[GameInput::ButtonRight].is_down) input_vec.x = 1.0f;
      if (inputs.buttons[GameInput::ButtonUp].is_down) input_vec.y = 1.0f;
      if (inputs.buttons[GameInput::ButtonDown].is_down) input_vec.y = -1.0f;

      input_vec = glm::normalize(input_vec);

      static const float velocity = 20.0f;

      glm::vec2 velocity_vec = input_vec * velocity;

      std::cout << "Player: [INFO] input processing result, velocity=" << velocity_vec << std::endl;

      // TODO: Properly move player
    });

    scene.m_objects.push_back(std::move(player));

  }


  void game2d_update(Game2D* game, GameInput* input) {
    assert(game);

    std::unique_ptr<CollisionDetectionStrategy2D> collision_detector = std::make_unique<CollisionDetectionStrategy2DUseAABB>();
    std::unique_ptr<DistanceDetectorStrategy2D> distance_detector = std::make_unique<DistanceDetectorStrategy2DUsePythagorean>();

    Scene2D& scene = game->m_scenes.front();
    Renderer2D& renderer = *game->m_renderer;

    GameObject2D& player = scene.get_player();

    player.process_inputs(*input);

    std::vector<GameObject2D*> close_objects = find_close_objects_related_to(player, scene.m_objects, 3, *distance_detector);

    for (auto object: close_objects) {
      bool collide = collision_detector->is_collide(*player.m_collision_body, *object->m_collision_body);
      if (collide) {
        collision_detector->resolve_collision(*player.m_collision_body, *object->m_collision_body);
      }
    }

    for (auto const& object : scene.m_objects) {
      if (object.m_visual_body)
        renderer.draw(*object.m_visual_body);
    }
  }
}

GameObject2D& Scene2D::get_player(std::string name) {
  return *std::find_if(m_objects.begin(), m_objects.end(),
  [name] (auto const& object) {
    return object.get_name() == name;
  });
}

std::vector<GameObject2D*> find_close_objects_related_to(GameObject2D const& the, std::vector<GameObject2D> objects, float distance, DistanceDetectorStrategy2D& distance_detector) {
  std::vector<GameObject2D*> result {};

  for (GameObject2D & object : objects) {
    if (not object.m_collision_body) continue;
    if (distance > distance_detector.distance_between(*the.m_collision_body, *object.m_collision_body)) {
      result.push_back(&object);
    }
  }
  return result;
}