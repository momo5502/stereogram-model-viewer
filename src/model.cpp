#include "std_include.hpp"

#include "model.hpp"
#include "context_saver.hpp"

model::model(const tinyobj::attrib_t& attributes, const std::vector<surface>& surfaces, const std::vector<texture>& textures)
{
	context_saver _;

	this->create_vertex_buffer(attributes);

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.data());

	this->texture_buffers.push_back(texture);
}

void model::create_vertex_buffer(const tinyobj::attrib_t& attributes)
{
	glGenBuffers(1, &this->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * attributes.vertices.size(), attributes.vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(double) * attributes.texcoords.size(), attributes.texcoords.data(), GL_STATIC_DRAW);

	if (attributes.vertices.size() / 3 != attributes.texcoords.size() / 2)
	{
		MessageBoxA(0, "WUT", 0, 0);
	}

	glGenVertexArrays(1, &this->vertex_array);
	glBindVertexArray(this->vertex_array);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, this->uv_buffer);
	glVertexAttribPointer(1, 2, GL_DOUBLE, GL_FALSE, 0, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void model::create_index_buffer(const surface& surf)
{
	unsigned int* index_array = reinterpret_cast<unsigned int*>(malloc(sizeof(unsigned int) * surf.indices.size()));

	for (size_t i = 0; i < surf.indices.size(); ++i)
	{
		index_array[i] = static_cast<unsigned int>(surf.indices[i].vertex_index);
	}

	model::faces faces_buffer;
	faces_buffer.count = static_cast<int>(surf.indices.size());

	glGenBuffers(1, &faces_buffer.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_buffer.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * surf.indices.size(), index_array, GL_STATIC_DRAW);

	free(index_array);

	this->faces_buffers.push_back(std::move(faces_buffer));
}

model::~model()
{
	glDeleteVertexArrays(1, &this->vertex_array);
	glDeleteBuffers(1, &this->uv_buffer);
	glDeleteBuffers(1, &this->vertex_buffer);

	for (auto& faces : this->faces_buffers)
	{
		glDeleteBuffers(1, &faces.index_buffer);
	}
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

	this->shader_program->use();

	glBindVertexArray(this->vertex_array);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	for (size_t i = 0; i < this->faces_buffers.size() && i < this->texture_buffers.size(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, this->texture_buffers[i]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->faces_buffers[i].index_buffer);
		glDrawElements(GL_TRIANGLES, this->faces_buffers[i].count, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void model::create_shader()
{
	static auto vertex_shader_source =
		"#version 400\n"

		"layout(location = 0) in vec3 vertex_position;"
		"layout(location = 1) in vec2 vertex_uv;"

		"out vec2 uv;"

		"void main(void)"
		"{"
		"	uv = vertex_uv;"
		"	gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position, 1.0);"
		"}";

	static auto fragment_shader_source =
		"in vec2 uv;"
		"uniform sampler2D texture_sampler;"

		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(texture_sampler, vec2(uv.x, -uv.y));"
		"}";

	this->shader_program = std::make_unique<shader>(vertex_shader_source, fragment_shader_source, std::vector<std::string>{ "vertex_position", "vertex_uv" });
}