#pragma once

#include "shader.hpp"
#include <paintable.hpp>

class camera;

class model : public paintable
{
public:
	class light
	{
	public:
		light(glm::vec3 origin_, glm::vec3 color_) : origin(origin_), color(color_) {}
		~light()
		{
			if (depthMap) {
				glDeleteTextures(1, &depthMap);
			}
		}

		glm::vec3 origin;
		glm::vec3 color;
		unsigned int depthMap = 0;
	};
	
	class texture
	{
	public:
		std::vector<std::vector<unsigned char>> data;
		int width{};
		int height{};
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

	std::vector<std::unique_ptr<light>> lights;

	std::unique_ptr<shader> shader_program;
	std::unique_ptr<shader> shader_program_no;
	std::unique_ptr<shader> shadow_map;

	camera* cam = nullptr;

	void create_vertex_buffer(const std::vector<model::vec<3>>& vertices,
		const std::vector<model::vec<3>>& normals,
		const std::vector<model::vec<2>>& uvs);
	void create_index_buffer(const surface& surf);
	void create_texture(const texture& tex);
	void create_texture_2d(const texture& tex);
	void create_texture_cube(const texture& tex);
	void create_shader();

	void generate_lightmap(light& light);
};
