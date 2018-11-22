#pragma once

#include "shader.hpp"
#include <paintable.hpp>

class camera;

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

	template<size_t len>
	class vec
	{
	public:
		vec()
		{
			std::memset(val, 0, sizeof(val));
		}

		double val[len];
	};

	class surface
	{
	public:
		std::vector<unsigned int> indices;
	};

	model(const std::vector<model::vec<3>>& vertices,
		const std::vector<model::vec<3>>& normals,
		const std::vector<model::vec<2>>& uvs,
		const std::vector<surface>& surfaces,
		const std::vector<texture>& textures);
	~model() override;

	void paint() override;
	void set_camera(camera* cam);

private:
	class faces
	{
	public:
		GLuint index_buffer = 0;
		int count = 0;
	};

	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint normal_buffer = 0;
	GLuint uv_buffer = 0;

	std::vector<GLuint> texture_buffers;
	std::vector<faces> faces_buffers;

	std::vector<int> num_faces;

	std::unique_ptr<shader> shader_program;
	std::unique_ptr<shader> shader_program_no;

	camera* cam = nullptr;

	void create_vertex_buffer(const std::vector<model::vec<3>>& vertices,
		const std::vector<model::vec<3>>& normals,
		const std::vector<model::vec<2>>& uvs);
	void create_index_buffer(const surface& surf);
	void create_texture(const texture& tex);
	void create_shader();
};
