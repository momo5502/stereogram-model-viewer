#pragma once

#define GLSL(version, shader)  "#version " #version "\n" #shader

class shader
{
public:
	shader(std::string vertex_source, std::string fragment_source, std::vector<std::string> attributes = {});
	~shader();

	void use() const;
	GLuint get() const;

private:
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	GLuint shader_program = 0;
};
