#pragma once

#include "model.hpp"

class obj_loader
{
public:
	obj_loader(std::string path);
	~obj_loader();

	model get_model();

private:
	std::string file_path;

	std::vector<glm::dvec4> vertices;
	std::vector<glm::dvec2> uvs;
	std::vector<std::array<int,3>> faces;

	std::vector<tinyobj::shape_t> shapes;

	void parse_file();

	void vertex_callback(tinyobj::real_t x, tinyobj::real_t y, tinyobj::real_t z, tinyobj::real_t w);
	static void vertex_callback_static(void *user_data, tinyobj::real_t x, tinyobj::real_t y, tinyobj::real_t z, tinyobj::real_t w);

	void index_callback(tinyobj::index_t *indices, int num_indices);
	static void index_callback_static(void *user_data, tinyobj::index_t *indices, int num_indices);
};
