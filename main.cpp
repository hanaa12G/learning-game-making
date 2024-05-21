#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ErrorOr.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "Camera.hpp"

void OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* userparam) {
  fprintf(stderr, "GL: message=%s\n", message);
}

int initGL(GLuint& program) {
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(OpenGLDebugMessageCallback, 0);
  glEnable(GL_DEPTH_TEST);
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

ErrorOr<ShaderProgram> SetupShaderProgram(char const* vertex_shader_path, char const* fragment_shader_path) {
  auto shader_1 = TRY(ShaderProgram::create());
  TRY(shader_1.UseVertexShader(vertex_shader_path));
  TRY(shader_1.UseFragmentShader(fragment_shader_path));

  return std::move(shader_1);

}


int main() {

  SDL_Window *window;
  GLuint shader_program = 0;
  if (0 != init(&window, shader_program)) {
    return 1;
  }

  auto program_1_r = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader_basic.glsl",
      "/home/wsluser/Workspace/game/runtime/fragment_shader_basic.glsl");
  if (program_1_r.HasError()) {
    return 1;
  }
  auto program_1 = program_1_r.ReleaseValue();
  if (program_1.Enable().HasError()) {
    return 1;
  }


  auto program_2_r = SetupShaderProgram(
      "/home/wsluser/Workspace/game/runtime/vertex_shader.glsl",
      "/home/wsluser/Workspace/game/runtime/fragment_shader.glsl");
  if (program_2_r.HasError()) {
    return 1;
  }
  auto program_2 = program_2_r.ReleaseValue();
  if (program_2.Enable().HasError()) {
    return 1;
  }

  SDL_Event e;
  bool quit = false;


  Camera camera;

  GLuint VAO[2];
  GLuint VBO[2];
  GLuint EBO[2];


  glGenVertexArrays(2, VAO);
  glGenBuffers(2, VBO);
  glGenBuffers(1, EBO);


  GLuint texture;
  [&texture]  {
    fprintf(stdout, "Get texture\n");
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int image_width, image_height, nr_channel;
    unsigned char* image_data = stbi_load("/home/wsluser/Workspace/game/runtime/wall.jpg", &image_width, &image_height, &nr_channel, 0);

    if (image_data) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      fprintf(stderr, "Can't open image\n");
    }
    stbi_image_free(image_data);
  } ();


  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

  glBufferData(GL_ARRAY_BUFFER, 
      sizeof(sample_models::CubeWithBrickTexture::vertices),
      sample_models::CubeWithBrickTexture::vertices,
      GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      sizeof(sample_models::CubeWithBrickTexture::indices),
      sample_models::CubeWithBrickTexture::indices,
      GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(VAO[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER,
      sizeof(sample_models::Plane::vertices),
      sample_models::Plane::vertices,
      GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);



  program_2.Use();

  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(program_2.program_id, "texture0"), 0);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)640 / (float)480, 0.1f, 100.0f);
  program_2.SetUniformMatrix("projection", projection);


  float degree = 0.0f;
  float move_x = 0.0f;
  float move_y = 0.0f;
  auto draw = [&]() {


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    program_2.Use();
    program_2.SetUniformMatrix("view", camera.GetViewMatrix());


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO[0]);
    for (int i = 0; i < 10; ++i) {
      float v = (float) i / (3.14 * 2);
      glm::mat4 trans(1.0f);
      trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
      trans = glm::translate(trans, glm::vec3(2 * std::sin(v), i * std::cos(v) , (float) (i % 2)));
      program_2.SetUniformMatrix("transform", trans);


      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    program_1.Use();
    program_1.SetUniformMatrix("view", camera.GetViewMatrix());
    {
      GLuint transform_loc = glGetUniformLocation(program_1.program_id, "transform");
      glm::mat4 trans(1.0f);
      trans = glm::scale(trans, glm::vec3(10.0f, 1.0f, 10.0f));

      program_1.SetUniformMatrix("transform", trans);
      program_1.SetUniformMatrix("projection", projection);
    }
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    SDL_GL_SwapWindow(window);
  };

  bool mouse_down = false;
  int start_x = 0;
  int start_y = 0;
  while (!quit) {
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT)
      quit = true;
    if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        quit = true;
      else {
        switch (e.key.keysym.sym) {
          case 'a':
            camera.Move(CameraMovement::Left, 1);
            break;
          case 'd':
            camera.Move(CameraMovement::Right, 1);
            break;
          case 'w':
            camera.Move(CameraMovement::Forward, 1);
            break;
          case 's':
            camera.Move(CameraMovement::Backward, 1);
            break;
          case 'q':
            camera.Move(CameraMovement::FlightUp, 1);
            break;
          case 'e':
            camera.Move(CameraMovement::FlightDown, 1);
            break;
        }
      }
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
      mouse_down = true;
      start_x = e.button.x;
      start_y = e.button.y;
    }
    else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
      mouse_down = false;
      move_x = 0.0f;
      move_y = 0.0f;
    }
    else if (e.type == SDL_MOUSEMOTION && mouse_down) {
      int x = e.button.x;
      int y = e.button.y;

      int dx = x - start_x;
      int dy = y - start_y;

      start_x = x;
      start_y = y;

      camera.Rotate(dx, dy);

      // fprintf(stdout, "camera: %f %f %f\n", camera.location.x, camera.location.y, camera.location.z);
    }

    draw();
  }

  glDeleteBuffers(2, VBO);
  glDeleteVertexArrays(2, VAO);
  SDL_Quit();
  return 0;
}
