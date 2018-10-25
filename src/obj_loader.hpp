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
	std::vector<std::array<int,3>> faces;

	std::string temporary_number;
	std::vector<std::string> temporary_values;

	void parse_file();
	void parse_line(std::string line);

	void parse_face(std::string line);
	void parse_vertex(std::string line);
};
