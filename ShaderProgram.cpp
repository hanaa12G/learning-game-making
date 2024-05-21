#include "ShaderProgram.hpp"
#include "ErrorOr.hpp"
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

#include <fstream>
#include <iostream>


ShaderError::ShaderError(ShaderError::Type type, char const* message) : m_type {type} {
  std::strncpy(m_message, message, 1024);
}


ErrorOr<ShaderProgram> ShaderProgram::create() {
  ShaderProgram program;
  program.program_id = glCreateProgram();
  if (!program.program_id) return {};

  return program;
}

ShaderProgram::~ShaderProgram() {
  std::cout << "Destroy shader program " << this << " " << program_id << std::endl;
  if (m_vertex_shader) glDeleteShader(m_vertex_shader);
  if (m_fragment_shader) glDeleteShader(m_fragment_shader);
  glDeleteProgram(program_id);
}

ErrorOr<void, ShaderError> ShaderProgram::UseVertexShader(char const* path) {
  GLint shader_compiled = GL_FALSE;
  GLuint vertex_shader = 0;
  GLint link_success = GL_FALSE;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  std::fstream file (path, std::ios::in);
  std::istreambuf_iterator begin(file);
  std::string source (begin, {});
  const GLchar* source_p = source.c_str();
  glShaderSource(vertex_shader, 1, &source_p, 0);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_compiled);
  if (shader_compiled != GL_TRUE) { 
    char info[1024] = {};
    GLsizei outlen = 0;
    glGetShaderInfoLog(vertex_shader, 1024, &outlen, info);
    return ShaderError(ShaderError::CompileShader, info);
  }

  m_vertex_shader = vertex_shader;
  glAttachShader(program_id, m_vertex_shader);
  return {};
}


ErrorOr<void, ShaderError> ShaderProgram::UseFragmentShader(char const* path) {
  GLint shader_compiled = GL_FALSE;
  GLuint fragment_shader = 0;
  GLint link_success = GL_FALSE;

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  std::fstream file (path, std::ios::in);
  std::istreambuf_iterator begin(file);
  std::string source (begin, {});
  const GLchar* source_p = source.c_str();
  glShaderSource(fragment_shader, 1, &source_p, 0);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_compiled);
  if (shader_compiled != GL_TRUE) { 
    char info[1024] = {};
    GLsizei outlen = 0;
    glGetShaderInfoLog(fragment_shader, 1024, &outlen, info);
    return ShaderError(ShaderError::CompileShader, info);
  }
  m_fragment_shader = fragment_shader;
  glAttachShader(program_id, m_fragment_shader);
  return {};
}

ErrorOr<void, ShaderError> ShaderProgram::Enable() {
  glLinkProgram(program_id);
  if (m_vertex_shader) glDeleteShader(m_vertex_shader);
  if (m_fragment_shader) glDeleteShader(m_fragment_shader);
  return {};
}


void ShaderProgram::Use() {
  glUseProgram(program_id);
  GLint success = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);
  if (!success) {
    char info[1024] = {};
    GLsizei outlen = 0;
    glGetProgramInfoLog(program_id, 1024, &outlen, info);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info << std::endl;
  }
}


ErrorOr<void, ShaderError> ShaderProgram::SetUniformValue_(char const* name, float v1, float v2, float v3, float v4) {
  int loc = glGetUniformLocation(program_id, name);
  if (loc == -1) return ShaderError(ShaderError::InvalidName, name);

  glUniform4f(loc, v1, v2, v3, v4);
  return {};
}

template<>
ErrorOr<void, ShaderError> ShaderProgram::SetUniformMatrix<glm::mat4>(char const* name, glm::mat4 m, bool transpose, unsigned) {
  int loc = glGetUniformLocation(program_id, name);
  if (loc == -1) return ShaderError(ShaderError::InvalidName, name);

  glUniformMatrix4fv(loc, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(m));
}
