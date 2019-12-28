#pragma once

#include "model.hpp"

class obj_loader
{
public:
	obj_loader();
	obj_loader(std::string path);
	~obj_loader();

	model get_model();

private:
	std::filesystem::path file_path;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::vector<model::surface> surfaces;
	std::vector<model::texture> textures;

	std::vector<model::vec<3>> vertices;
	std::vector<model::vec<3>> normals;
	std::vector<model::vec<2>> uvs;

	std::map<int, std::map<int, std::map<int, unsigned int>>> index_mapping;

	void parse_file();
	void load_textures();
	void sort_surfaces();

	std::vector<unsigned char> load_texture(const std::filesystem::path& path, int* width, int* height) const;

	unsigned int load_index(const tinyobj::index_t& index);
	unsigned int load_index(int vertex, int normal, int texture);
};
