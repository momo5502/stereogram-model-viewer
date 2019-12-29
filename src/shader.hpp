#pragma once

#define GLSL(version, shader)  "#version " #version "\n" #shader

class shader
{
public:
	shader(const std::string& vertex_source, const std::string& fragment_source, const std::vector<std::string>& attributes = {});
	shader(const std::string& vertex_source, const std::string& fragment_source, const std::string& geometry_source, const std::vector<std::string>& attributes = {});
	~shader();

	void use() const;
	GLuint get() const;

private:
	GLuint shader_program = 0;
};
