#pragma once

#include "model.hpp"

class obj_loader
{
public:
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

	void parse_file();
	void load_textures();
	void sort_surfaces();
};
