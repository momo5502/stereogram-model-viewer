#include "std_include.hpp"

#include "shader.hpp"

shader::shader(std::string vertex_source, std::string fragment_source, std::vector<std::string> attributes)
{
	char* vertex_shader_source = vertex_source.data();
	char* fragment_shader_source = fragment_source.data();

	this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(this->vertex_shader);

	char log[1024];
	int len = 0;
	glGetShaderInfoLog(this->vertex_shader, 1024, &len, log);
	OutputDebugStringA("Vertex shader log:\n");
	OutputDebugStringA(log);
	OutputDebugStringA("\n\n");

	this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(this->fragment_shader);

	glGetShaderInfoLog(this->fragment_shader, 1024, &len, log);

	OutputDebugStringA("Fragment shader log:\n");
	OutputDebugStringA(log);
	OutputDebugStringA("\n\n");

	this->shader_program = glCreateProgram();
	glAttachShader(this->shader_program, this->fragment_shader);
	glAttachShader(this->shader_program, this->vertex_shader);

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glBindAttribLocation(this->shader_program, GLuint(i), attributes[i].data());
	}

	glLinkProgram(this->shader_program);
}

shader::~shader()
{
	glDeleteProgram(this->shader_program);
	glDeleteShader(this->fragment_shader);
	glDeleteShader(this->vertex_shader);
}

void shader::use()
{
	if (this->shader_program)
	{
		glUseProgram(this->shader_program);
	}
}
