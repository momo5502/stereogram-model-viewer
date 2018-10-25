#pragma once

class shader
{
public:
	shader(std::string vertex_source, std::string fragment_source, std::vector<std::string> attributes = {});
	~shader();

	void use();

private:
	GLuint vertex_shader = 0;
	GLuint fragment_shader = 0;
	GLuint shader_program = 0;
};
