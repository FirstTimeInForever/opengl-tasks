#include "opengl_shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace
{
   std::string read_shader_code(const std::string& filename) {
      try {
         std::ifstream file(filename);
         file.exceptions(std::ifstream::badbit);
         return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      }
      catch (std::exception const& exc) {
         std::cerr << "Error reading shader file: " << exc.what() << std::endl;
      }
   }
}

shader_t::shader_t(const std::string& vertex_code_fname, const std::string& fragment_code_fname) {
   const auto vertex_code = read_shader_code(vertex_code_fname);
   const auto fragment_code = read_shader_code(fragment_code_fname);
   compile(vertex_code, fragment_code);
   link();
}

shader_t::~shader_t() {
}

void shader_t::compile(const std::string& vertex_code, const std::string& fragment_code)
{
   const char* vcode = vertex_code.c_str();
   vertex_id_ = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_id_, 1, &vcode, NULL);
   glCompileShader(vertex_id_);

   const char* fcode = fragment_code.c_str();
   fragment_id_ = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_id_, 1, &fcode, NULL);
   glCompileShader(fragment_id_);
   check_compile_error();
}

void shader_t::link() {
   program_id_ = glCreateProgram();
   glAttachShader(program_id_, vertex_id_);
   glAttachShader(program_id_, fragment_id_);
   glLinkProgram(program_id_);
   check_linking_error();
   glDeleteShader(vertex_id_);
   glDeleteShader(fragment_id_);
}

void shader_t::use() {
   glUseProgram(program_id_);
}

template<>
void shader_t::set_uniform<int>(const std::string& name, int val) {
   glUniform1i(glGetUniformLocation(program_id_, name.c_str()), val);
}

template<>
void shader_t::set_uniform<bool>(const std::string& name, bool val) {
   glUniform1i(glGetUniformLocation(program_id_, name.c_str()), val);
}

template<>
void shader_t::set_uniform<float>(const std::string& name, float val) {
   glUniform1f(glGetUniformLocation(program_id_, name.c_str()), val);
}

template<>
void shader_t::set_uniform<float>(const std::string& name, float val1, float val2) {
   glUniform2f(glGetUniformLocation(program_id_, name.c_str()), val1, val2);
}

template<>
void shader_t::set_uniform<float>(const std::string& name, float val1, float val2, float val3) {
   glUniform3f(glGetUniformLocation(program_id_, name.c_str()), val1, val2, val3);
}

template<>
void shader_t::set_uniform<float*>(const std::string& name, float* val) {
   glUniformMatrix4fv(glGetUniformLocation(program_id_, name.c_str()), 1, GL_FALSE, val);
}

void shader_t::check_compile_error() {
   int success;
   char infoLog[1024];
   glGetShaderiv(vertex_id_, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(vertex_id_, 1024, NULL, infoLog);
      std::cerr << "Error compiling Vertex shader_t:\n" << infoLog << std::endl;
   }
   glGetShaderiv(fragment_id_, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(fragment_id_, 1024, NULL, infoLog);
      std::cerr << "Error compiling Fragment shader_t:\n" << infoLog << std::endl;
   }
}

void shader_t::check_linking_error() {
   int success;
   char infoLog[1024];
   glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(program_id_, 1024, NULL, infoLog);
      std::cerr << "Error Linking shader_t Program:\n" << infoLog << std::endl;
   }
}
