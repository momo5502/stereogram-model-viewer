#pragma once

#include "shader.hpp"
#include <paintable.hpp>

class model : public paintable
{
public:
	class texture
	{
	public:
		std::vector<unsigned char> data;
		unsigned width;
		unsigned height;
	};

	class surface
	{
	public:
		std::vector<tinyobj::index_t> indices;
	};

	model(const tinyobj::attrib_t& attributes, const std::vector<surface>& surfaces, const std::vector<texture>& textures);
	~model() override;

	void paint() override;

private:
	class faces
	{
	public:
		GLuint index_buffer = 0;
		int count = 0;
	};

	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint uv_buffer = 0;

	std::vector<GLuint> texture_buffers;
	std::vector<faces> faces_buffers;

	std::vector<int> num_faces;

	std::unique_ptr<shader> shader_program;

	void create_vertex_buffer(const tinyobj::attrib_t& attributes);
	void create_index_buffer(const surface& surf);
	void create_texture(const texture& tex);
	void create_shader();
};
