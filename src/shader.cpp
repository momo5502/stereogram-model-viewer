#include "std_include.hpp"

#include "shader.hpp"

shader::shader(std::string vertex_source, std::string fragment_source, std::vector<std::string> attributes)
{
	char* vertex_shader_source = vertex_source.data();
	char* fragment_shader_source = fragment_source.data();

	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader);

	char log[1024];
	int len = 0;
	glGetShaderInfoLog(vertex_shader, sizeof log, &len, log);
	OutputDebugStringA("Vertex shader log:\n");
	OutputDebugStringA(log);
	OutputDebugStringA("\n\n");

	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);

	glGetShaderInfoLog(fragment_shader, sizeof log, &len, log);

	OutputDebugStringA("Fragment shader log:\n");
	OutputDebugStringA(log);
	OutputDebugStringA("\n\n");

	this->shader_program = glCreateProgram();
	glAttachShader(this->shader_program, fragment_shader);
	glAttachShader(this->shader_program, vertex_shader);

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glBindAttribLocation(this->shader_program, GLuint(i), attributes[i].data());
	}

	glLinkProgram(this->shader_program);
	glValidateProgram(this->shader_program);

	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);
}

shader::~shader()
{
	glDeleteProgram(this->shader_program);
}

void shader::use() const
{
	if (this->shader_program)
	{
		glUseProgram(this->shader_program);
	}
}

GLuint shader::get() const
{
	return this->shader_program;
}
