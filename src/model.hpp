#pragma once

#include <paintable.hpp>

class model : public paintable
{
public:
	model(const std::vector<glm::dvec4>& vertices, const std::vector<std::array<int, 3>>& faces);
	~model() override;

	void paint() override;

private:
	GLuint index_buffer = 0;
	GLuint vertex_buffer = 0;

	int num_faces;

	void create_vertex_buffer(const std::vector<glm::dvec4>& vertices);
	void create_index_buffer(const std::vector<std::array<int, 3>>& faces);
};
