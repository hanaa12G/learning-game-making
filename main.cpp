#include "Renderer2DOpenGL.hpp"
#include <SDL2/SDL.h>

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <chrono>


#include "ErrorOr.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "OpenGLCamera.hpp"
#include "game/Game.hpp"
#include "Renderer2DOpenGL.hpp"

namespace chrono = std::chrono;
using namespace std::literals;

ErrorOr<ShaderProgram> SetupShaderProgram(char const* vertex_shader_path, char const* fragment_shader_path) {
  auto shader_1 = TRY(ShaderProgram::create());
  TRY(shader_1.UseVertexShader(vertex_shader_path));
  TRY(shader_1.UseFragmentShader(fragment_shader_path));

  return std::move(shader_1);

}
struct OpenGLRenderer : public Renderer {

  virtual void SetActiveCamera(Camera& camera) override {
    this->camera.direction = camera.direction;
    this->camera.location = camera.location;
    this->camera.up = camera.up;
  }

  virtual void Draw(GameObject& obj) override {
    if (std::string("Dot") == obj.Type()) {
      Dot& dot = dynamic_cast<Dot&>(obj);
      if (drawable_resources.count(dot.Type()) == 0) {
        LoadResource(dot);
      }
      DrawDot(dot);
    }
    else if (std::string("Line") == obj.Type()) {
      Line& line = dynamic_cast<Line&>(obj);
      if (drawable_resources.count(line.Type()) == 0) {
        LoadResource(line);
      }
      DrawLine(line);
    }
    else if (std::string("Rectangle") == obj.Type()) {
      Rectangle& rectangle = dynamic_cast<Rectangle&>(obj);
      if (drawable_resources.count(rectangle.Type()) == 0) {
        LoadResource(rectangle);
      }
      DrawRectangle(rectangle);
    }
    else {
      if (drawable_resources.count(obj.Type()) == 0) {
        LoadResource(obj);
      }
      DrawCube(obj);
    }
  }

  virtual void DrawBatch(std::vector<GameObject*>& obj) override {
    if (obj.empty()) return;
    if (drawable_resources.count(BatchNameOf(*obj.front())) == 0) {
      LoadBatchResource(*obj.front());
    }
    auto& resource = drawable_resources[BatchNameOf(*obj.front())];
    resource.program.Use();


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resource.texture);
    

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);

    resource.program.SetUniformMatrix("projection", projection);
    resource.program.SetUniformMatrix("view", camera.GetViewMatrix());
    std::vector<glm::mat4> transform_list;
    int count = 0;
    while (count < obj.size()) {
      int i = 0;
      for (i = 0; i < 512 && count + i < obj.size(); ++i) {
        GameObject* game_object = obj[count + i];

        glm::mat4 trans(1.0f);
        trans = glm::translate(trans, game_object->pos);
        std::string name = std::string("transform") + "[" + std::to_string(i) + "]";
        resource.program.SetUniformMatrix(name.c_str(), trans);
      }
      //std::cout << "Draw " << i << "object" << std::endl;
      glBindVertexArray(resource.vao);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 36, i);
      count += i;
    }

  }

  void DrawCube(GameObject& obj) {
    auto& resource = drawable_resources[obj.Type()];
    resource.program.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, resource.texture);


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);
    resource.program.SetUniformMatrix("projection", projection);
    resource.program.SetUniformMatrix("view", camera.GetViewMatrix());
    
    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, obj.pos);

    resource.program.SetUniformMatrix("transform", trans);

    glBindVertexArray(resource.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  void DrawDot(Dot& dot) {
    auto& resource = drawable_resources[dot.Type()];
    resource.program.Use();

    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, dot.pos);
    resource.program.SetUniformMatrix("transform", trans);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glPointSize(10);
    glBindVertexArray(resource.vao);
    glDrawArrays(GL_POINTS, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPointSize(1);
  }

  void DrawLine(Line& line) {
    auto& resource = drawable_resources[line.Type()];
    resource.program.Use();

    // How we draw a line:
    // - Scale a unit vector (1, 0, 0) to the length of the line
    // - Rotate that vector to correct angle (will be parallel with target vector)
    // - Move that vector to the position of the target line
    glm::mat4 trans(1.0f);
    // trans = glm::scale(trans, glm::vec3(glm::length(line.Ray()), glm::length(line.Ray()), glm::length(line.Ray())));
    glm::vec3 ox(1.0f, 0.0f, 0.0f);
    float det = line.Ray().y;
    float dot = glm::dot(line.Ray(), ox);
    float angle = glm::atan(det, dot);
    // std::cout << "angle: " << glm::degrees(angle) << std::endl;

    trans = glm::translate(trans, line.pos);
    trans = glm::rotate(trans, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(glm::length(line.Ray()), 0.0f, 0.0f));

    resource.program.SetUniformMatrix("transform", trans);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(resource.vao);
    glDrawArrays(GL_LINES, 0, 2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void DrawRectangle(Rectangle& rect) {
    auto& resource = drawable_resources[rect.Type()];
    resource.program.Use();

    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, glm::vec3 {rect.pos.x, rect.pos.y, 0.0 });
    trans = glm::scale(trans, glm::vec3(rect.width, rect.height, 0.0f));
    trans = glm::translate(trans, glm::vec3 {-0.5f, -0.5f, 0.0f});
    resource.program.SetUniformMatrix("transform", trans);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(resource.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void SetupShader(Dot& dot) {
    auto& resource = drawable_resources[dot.Type()];
    resource.program = SetupShaderProgram(
    "/home/wsluser/Workspace/game/runtime/2d_vertex_shader.glsl",
    "/home/wsluser/Workspace/game/runtime/2d_fragment_shader.glsl").ReleaseValue();

    resource.program.Use();
  }

  void LoadBatchResource(GameObject& obj) {
    auto& resource = drawable_resources[BatchNameOf(obj)];
    resource.program = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader_batch.glsl",
       "/home/wsluser/Workspace/game/runtime/fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int image_width, image_height, nr_channel;
    unsigned char* image_data = stbi_load(obj.Texture(), &image_width, &image_height, &nr_channel, 0);

    if (image_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      fprintf(stderr, "Can't open image\n");
    }
    stbi_image_free(image_data);
    resource.texture =  texture;

    auto vertices = ModelConstruction::Cube(
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {1.0, 0.0, 1.0},
      {0.0, 0.0, 1.0},
      {0.0, 1.0, 0.0},
      {1.0, 1.0, 0.0},
      {1.0, 1.0, 1.0},
      {0.0, 1.0, 1.0}
    );
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    


    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;
  }

  void LoadResource(GameObject& obj) {
    auto& resource = drawable_resources[obj.Type()];
    resource.program = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader.glsl",
      "/home/wsluser/Workspace/game/runtime/fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int image_width, image_height, nr_channel;
    unsigned char* image_data = stbi_load(obj.Texture(), &image_width, &image_height, &nr_channel, 0);

    if (image_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      fprintf(stderr, "Can't open image\n");
    }
    stbi_image_free(image_data);
    resource.texture =  texture;

    auto vertices = ModelConstruction::Cube(
      {0.0, 0.0, 0.0},
      {1.0, 0.0, 0.0},
      {1.0, 0.0, 1.0},
      {0.0, 0.0, 1.0},
      {0.0, 1.0, 0.0},
      {1.0, 1.0, 0.0},
      {1.0, 1.0, 1.0},
      {0.0, 1.0, 1.0}
    );
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    


    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;

  }

  void LoadResource(Dot& dot) {
    auto& resource = drawable_resources[dot.Type()];
    resource.program = SetupShaderProgram(
        "/home/wsluser/Workspace/game/runtime/2d_vertex_shader.glsl",
        "/home/wsluser/Workspace/game/runtime/2d_fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();

    auto vertices = std::vector<float> {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;
  }

  void LoadResource(Line& line) {
    auto& resource = drawable_resources[line.Type()];
    resource.program = SetupShaderProgram(
        "/home/wsluser/Workspace/game/runtime/2d_vertex_shader.glsl",
        "/home/wsluser/Workspace/game/runtime/2d_fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();

    auto vertices = std::vector<float> {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      };
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;
  }

  void LoadResource(Rectangle& rect) {
    auto& resource = drawable_resources[rect.Type()];
    resource.program = SetupShaderProgram(
        "/home/wsluser/Workspace/game/runtime/2d_vertex_shader.glsl",
        "/home/wsluser/Workspace/game/runtime/2d_fragment_shader.glsl").ReleaseValue();
    resource.program.Enable();

    auto vertices = std::vector<float> {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,

      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };
    resource.model_data = vertices;

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, resource.model_data.size() * sizeof(float), resource.model_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    resource.vao = vao;
  }


  std::string BatchNameOf(GameObject& obj) {
    return std::string(obj.Type()) + "_batch";
  }


  struct DrawableResource {
    ShaderProgram program;
    GLuint texture;
    std::vector<float> model_data;
    GLuint vao;
  };

  std::map<std::string, DrawableResource> drawable_resources;
  OpenGLCamera camera;
  //ShaderProgram shader_program;
};

void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userparam) {
  fprintf(stderr, "GL: message=%s\n", message);
}

int initGL(GLuint& program) {
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(OpenGLDebugMessageCallback, 0);
  return 0;
}

int init(SDL_Window **window, GLuint& shader_program) {

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Can't init SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


  *window =
    SDL_CreateWindow("game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Can't create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_GLContext gl_context = SDL_GL_CreateContext(*window);
  glewExperimental = GL_TRUE;
  GLenum glerror = glewInit();
  if (glerror != GLEW_OK) {
    fprintf(stderr, "Initialize GLEW %s\n", glewGetErrorString(glerror));
    return 1;
  }

  if (SDL_GL_SetSwapInterval(1) < 0) {
    fprintf(stderr, "Set vsync: %s\n", SDL_GetError());
  }

  if (0 != initGL(shader_program)) {
    fprintf(stderr, "Initialize OpenGL failed\n");
    return 1;
  }

  return 0;
}


void CopyLastFrameInput(GameInput& input, GameInput& last_input) {
  input.buttons[GameInput::ButtonUp] = {.is_down = last_input.buttons[GameInput::ButtonUp].is_down, .transition_count = 0, .down_timestamp = last_input.buttons[GameInput::ButtonUp].down_timestamp};
  input.buttons[GameInput::ButtonDown] = {.is_down = last_input.buttons[GameInput::ButtonDown].is_down, .transition_count = 0};
  input.buttons[GameInput::ButtonLeft] = {.is_down = last_input.buttons[GameInput::ButtonLeft].is_down, .transition_count = 0};
  input.buttons[GameInput::ButtonRight] = {.is_down = last_input.buttons[GameInput::ButtonRight].is_down, .transition_count = 0};

  input.mouse.mouse_buttons[InputMouse::MouseButtonRight] = {.is_down = last_input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down, .transition_count = 0};
  input.mouse.mouse_buttons[InputMouse::MouseButtonLeft] = {.is_down = last_input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down, .transition_count = 0};

  input.mouse.x = last_input.mouse.x;
  input.mouse.y = last_input.mouse.y;
}

void HandleInputEvent(SDL_Event e, GameInput& input) {
  if (e.type == SDL_KEYDOWN) {
    if (e.key.repeat > 0) return;
    switch (e.key.keysym.sym) {
      case 'w':
      {
        bool was_down = input.buttons[GameInput::ButtonUp].is_down;
        bool is_down = true;
        input.buttons[GameInput::ButtonUp].is_down = is_down;
        input.buttons[GameInput::ButtonUp].transition_count += (int) (was_down != is_down);
        input.buttons[GameInput::ButtonUp].down_timestamp = chrono::high_resolution_clock::now();
        std::cout << "Button state is set" << std::endl;
      } break;
      case 's':
      {
        bool was_down = input.buttons[GameInput::ButtonDown].is_down;
        bool is_down = true;
        input.buttons[GameInput::ButtonDown].is_down = is_down;
        input.buttons[GameInput::ButtonDown].transition_count += (int) (was_down != is_down);
      } break;
      case 'a':
      {
        bool was_down = input.buttons[GameInput::ButtonLeft].is_down;
        bool is_down = true;
        input.buttons[GameInput::ButtonLeft].is_down = is_down;
        input.buttons[GameInput::ButtonLeft].transition_count += (int) (was_down != is_down);
      } break;
      case 'd':
      {
        bool was_down = input.buttons[GameInput::ButtonRight].is_down;
        bool is_down = true;
        input.buttons[GameInput::ButtonRight].is_down = is_down;
        input.buttons[GameInput::ButtonRight].transition_count += (int) (was_down != is_down);
      } break;
      case 'f':
      {
        bool was_down = input.buttons[GameInput::ButtonFront].is_down;
        bool is_down = true;
        input.buttons[GameInput::ButtonFront].is_down = is_down;
        input.buttons[GameInput::ButtonFront].transition_count += (int) (was_down != is_down);
      };
      default:
        break;
    }
  }
  else if (e.type == SDL_KEYUP) {
    switch (e.key.keysym.sym) {
      case 'w':
      {
        bool was_down = input.buttons[GameInput::ButtonUp].is_down;
        bool is_down = false;
        input.buttons[GameInput::ButtonUp].is_down = is_down;
        input.buttons[GameInput::ButtonUp].transition_count += (int) (was_down != is_down);
      } break;
      case 's':
      {
        bool was_down = input.buttons[GameInput::ButtonDown].is_down;
        bool is_down = false;
        input.buttons[GameInput::ButtonDown].is_down = is_down;
        input.buttons[GameInput::ButtonDown].transition_count += (int) (was_down != is_down);
      } break;
      case 'a':
      {
        bool was_down = input.buttons[GameInput::ButtonLeft].is_down;
        bool is_down = false;
        input.buttons[GameInput::ButtonLeft].is_down = is_down;
        input.buttons[GameInput::ButtonLeft].transition_count += (int) (was_down != is_down);
      } break;
      case 'd':
      {
        bool was_down = input.buttons[GameInput::ButtonRight].is_down;
        bool is_down = false;
        input.buttons[GameInput::ButtonRight].is_down = is_down;
        input.buttons[GameInput::ButtonRight].transition_count += (int) (was_down != is_down);
      } break;
      case 'f':
      {
        bool was_down = input.buttons[GameInput::ButtonFront].is_down;
        bool is_down = false;
        input.buttons[GameInput::ButtonFront].is_down = is_down;
        input.buttons[GameInput::ButtonFront].transition_count += (int) (was_down != is_down);
      } break;
      default:
        break;
    }
  }
  else if (e.type == SDL_MOUSEBUTTONDOWN) {
    switch(e.button.button) {
      case SDL_BUTTON_LEFT:
      {
        bool was_down = input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down;
        bool is_down = true;
        input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down = is_down;
        input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].transition_count += (int) (was_down != is_down);
      } break;
      case SDL_BUTTON_RIGHT:
      {
        bool was_down = input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down;
        bool is_down = true;
        input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down = is_down;
        input.mouse.mouse_buttons[InputMouse::MouseButtonRight].transition_count += (int) (was_down != is_down);
      } break;
    }
    input.mouse.x = e.button.x;
    input.mouse.y = e.button.y;
  }
  else if (e.type == SDL_MOUSEBUTTONUP) {
    switch(e.button.button) {
      case SDL_BUTTON_LEFT:
      {
        bool was_down = input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down;
        bool is_down = false;
        input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].is_down = is_down;
        input.mouse.mouse_buttons[InputMouse::MouseButtonLeft].transition_count += (int) (was_down != is_down);
      } break;
      case SDL_BUTTON_RIGHT:
      {
        bool was_down = input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down;
        bool is_down = false;
        input.mouse.mouse_buttons[InputMouse::MouseButtonRight].is_down = is_down;
        input.mouse.mouse_buttons[InputMouse::MouseButtonRight].transition_count += (int) (was_down != is_down);
      } break;
    }
    input.mouse.x = e.button.x;
    input.mouse.y = e.button.y;
  }
  else if (e.type == SDL_MOUSEMOTION) {
    input.mouse.x = e.button.x;
    input.mouse.y = e.button.y;
  }
}



int main() {

  SDL_Window *window;
  GLuint shader_program = 0;
  if (0 != init(&window, shader_program)) {
    return 1;
  }

  SDL_Event e;
  bool quit = false;

  float degree = 0.0f;
  float move_x = 0.0f;
  float move_y = 0.0f;

  bool mouse_down = false;
  int start_x = 0;
  int start_y = 0;

  Game game;
  game.viewport_width = 640;
  game.viewport_height = 480;
  GameInput input {};
  GameInput prev_input {};
  OpenGLRenderer renderer;

  Game2D game2d;
  game2d_init(&game2d);
  game2d.m_renderer = std::make_unique<Renderer2DOpenGL>();
  
  // renderer.shader_program = SetupShaderProgram("runtime/vertext_shader.glsl", "runtime/fragment_shader.glsl").ReleaseValue();
  auto frame_start = chrono::high_resolution_clock::now();
  auto fps = 30;
  auto frame_time = 1000ms / fps;
  auto time_start = chrono::high_resolution_clock::now();
  while (!quit) {
    frame_start = chrono::high_resolution_clock::now();
    CopyLastFrameInput(input, prev_input);
    while (SDL_PollEvent(&e)) {
      HandleInputEvent(e, input);
      if (e.type == SDL_QUIT)
        quit = true;
      if (e.type == SDL_KEYDOWN)
        if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
          quit = true;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto time_end = chrono::high_resolution_clock::now();
    float elapsed_time = chrono::duration_cast<chrono::duration<float>>(time_end - time_start).count();
    time_start = time_end;
    // game_update(game, renderer, input, elapsed_time);
    game2d_update(&game2d, &input, elapsed_time);

    SDL_GL_SwapWindow(window);
    prev_input = input;

    auto frame_end = chrono::high_resolution_clock::now();
    auto frame_duration = frame_end - frame_start;
    if (frame_duration < frame_time) {
      std::this_thread::sleep_for(frame_time - frame_duration);
    }

  }

  SDL_Quit();
  return 0;
}
