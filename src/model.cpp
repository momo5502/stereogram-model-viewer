#include "std_include.hpp"

#include "camera.hpp"
#include "model.hpp"
#include "context_saver.hpp"

model::model(const std::vector<model::vec<3>>& vertices,
	const std::vector<model::vec<3>>& normals,
	const std::vector<model::vec<2>>& uvs,
	const std::vector<surface>& surfaces,
	const std::vector<texture>& textures)
{
	context_saver _;

	this->create_vertex_buffer(vertices, normals, uvs);

	this->faces_buffers.reserve(surfaces.size() / 3);
	for (auto& surf : surfaces)
	{
		this->create_index_buffer(surf);
	}

	this->texture_buffers.reserve(textures.size());
	for (auto& tex : textures)
	{
		this->create_texture(tex);
	}

	this->create_shader();
}

void model::create_texture(const model::texture& tex)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	this->texture_buffers.push_back(texture);
}

void model::create_vertex_buffer(const std::vector<model::vec<3>>& vertices,
	const std::vector<model::vec<3>>& normals,
	const std::vector<model::vec<2>>& uvs)
{
	glGenBuffers(1, &this->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 2 * uvs.size(), uvs.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * 3 * normals.size(), normals.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &this->vertex_array);
	glBindVertexArray(this->vertex_array);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_buffer);
	glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void model::create_index_buffer(const surface& surf)
{
	model::faces faces_buffer;
	faces_buffer.count = static_cast<int>(surf.indices.size());

	glGenBuffers(1, &faces_buffer.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_buffer.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * surf.indices.size(), surf.indices.data(), GL_STATIC_DRAW);

	this->faces_buffers.push_back(std::move(faces_buffer));
}

model::~model()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->uv_buffer);
	glDeleteBuffers(1, &this->normal_buffer);
	glDeleteBuffers(1, &this->vertex_buffer);

	for (auto& faces : this->faces_buffers)
	{
		glDeleteBuffers(1, &faces.index_buffer);
	}
}

void model::set_camera(camera* _cam)
{
	this->cam = _cam;
}

void model::paint()
{
	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT_AND_BACK);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (GetKeyState(VK_CAPITAL) & 0x0001)
		this->shader_program_no->use();
	else
		this->shader_program->use();

	GLint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	GLint loc = glGetUniformLocation(program, "camera_position");
	if (loc != -1)
	{
		glm::vec3 pos = this->cam->get_position();
		glUniform3f(loc, pos[0], pos[1], pos[2]);
	}

	loc = glGetUniformLocation(program, "light_position");
	if (loc != -1)
	{
		glUniform3f(loc, 0.0f, 10000.0f, 0.0f);
	}

	loc = glGetUniformLocation(program, "light_color_ambient");
	if (loc != -1)
	{
		glUniform3f(loc, 0.0956127f, 0.0854841f, 0.0777573f);
	}

	loc = glGetUniformLocation(program, "light_color_diffuse");
	if (loc != -1)
	{
		glUniform3f(loc, 0.3f, 0.3f, 0.3f);
	}

	loc = glGetUniformLocation(program, "light_color_specular");
	if (loc != -1)
	{
		glUniform3f(loc, 0.1f, 0.1f, 0.1f);
	}

	loc = glGetUniformLocation(program, "shininess");
	if (loc != -1)
	{
		glUniform1f(loc, 1.0f);
	}

	glBindVertexArray(this->vertex_array);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (size_t i = 0; i < this->faces_buffers.size() && i < this->texture_buffers.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, this->texture_buffers[i]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faces_buffers[i].index_buffer);
		glDrawElements(GL_TRIANGLES, this->faces_buffers[i].count, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void model::create_shader()
{
	static auto vertex_shader_source =
		"#version 400\n"

		"in vec3 vertex_position;"
		"in vec2 vertex_uv;"
		"in vec3 vertex_normal;"

		"out vec2 uv;"
		"out vec3 normal;"
		"out vec3 vertex;"
		"out vec3 cam_pos;"
		"out vec3 light_pos;"
		"out vec3 light_color_specular_val;"
		"out vec3 light_color_ambient_val;"
		"out vec3 light_color_diffuse_val;"
		"out float shininess_val;"

		"uniform vec3 camera_position;"
		"uniform vec3 light_position;"
		"uniform vec3 light_color_specular;"
		"uniform vec3 light_color_ambient;"
		"uniform vec3 light_color_diffuse;"
		"uniform float shininess;"

		"void main(void)"
		"{"
		"	uv = vertex_uv;"
		"	normal = vertex_normal;"
		"	vertex = vertex_position;"
		"	cam_pos = camera_position;"
		"	light_pos = light_position;"
		"	light_color_specular_val = light_color_specular;"
		"	light_color_ambient_val = light_color_ambient;"
		"	light_color_diffuse_val = light_color_diffuse;"
		"	shininess_val = shininess;"
		"	gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 1.0);"
		"}";

	static auto fragment_shader_source =
		"in vec2 uv;"
		"in vec3 vertex;"
		"in vec3 normal;"
		"in vec3 cam_pos;"
		"in vec3 light_pos;"
		"in vec3 light_color_specular_val;"
		"in vec3 light_color_ambient_val;"
		"in vec3 light_color_diffuse_val;"
		"in float shininess_val;"

		"uniform sampler2D texture_sampler;"

		"float do_blinn_single_channel(float ks, float Iin, vec3 h)"
		"{"
		"	return Iin * ks * pow(dot(h, normal), shininess_val);"
		"}"

		"vec3 do_blinn_specular(vec3 color)"
		"{"
		"	vec3 h = normalize(light_pos) + normalize(cam_pos);"

		"	color.r = do_blinn_single_channel(color.r, light_color_specular_val.r, h);"
		"	color.g = do_blinn_single_channel(color.g, light_color_specular_val.g, h);"
		"	color.b = do_blinn_single_channel(color.b, light_color_specular_val.b, h);"
		"	return color;"
		"}"

		"vec3 do_blinn_ambient(vec3 color)"
		"{"
		"	return color + light_color_ambient_val;"
		"}"

		"vec3 do_blinn_diffuse(vec3 color)"
		"{"
		"	float scale = dot(normalize(light_pos), normalize(normal));"

		"	color.r = color.r * scale * light_color_diffuse_val.r;"
		"	color.g = color.g * scale * light_color_diffuse_val.g;"
		"	color.b = color.b * scale * light_color_diffuse_val.b;"
		"	return color;"
		"}"

		"vec3 do_blinn_shading(vec3 color)"
		"{"
		"	return do_blinn_ambient(color) + do_blinn_diffuse(color) + do_blinn_specular(color);"
		"}"

		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));"
		"	gl_FragColor.rgb = do_blinn_shading(gl_FragColor.rgb);"
		"}";

	this->shader_program = std::make_unique<shader>(vertex_shader_source, fragment_shader_source, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });




	static auto vertex_shader_source_no =
		"#version 400\n"

		"in vec3 vertex_position;"
		"in vec2 vertex_uv;"
		"in vec3 vertex_normal;"

		"out vec2 uv;"
		"out vec3 normal;"
		"out vec3 vertex;"

		"void main(void)"
		"{"
		"	uv = vertex_uv;"
		"	normal = vertex_normal;"
		"	vertex = vertex_position;"
		"	gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 1.0);"
		"}";

	static auto fragment_shader_source_no =
		"in vec2 uv;"
		"in vec3 vertex;"
		"in vec3 normal;"

		"uniform sampler2D texture_sampler;"

		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));"
		"}";

	this->shader_program_no = std::make_unique<shader>(vertex_shader_source_no, fragment_shader_source_no, std::vector<std::string>{ "vertex_position", "vertex_uv", "vertex_normal" });
}