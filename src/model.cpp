#include "std_include.hpp"

#include "model.hpp"

model::model(const std::vector<glm::dvec4>& vertices, const std::vector<std::array<int, 3>>& faces) :
	num_faces(static_cast<int>(faces.size()))
{
	this->create_vertex_buffer(vertices);
	this->create_index_buffer(faces);
}

void model::create_vertex_buffer(const std::vector<glm::dvec4>& vertices)
{
	float* vertex_array = reinterpret_cast<float*>(malloc(sizeof(float) * 3 * vertices.size()));

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		auto& vertex = vertices[i];

		for (int v = 0; v < 3; ++v)
		{
			vertex_array[i * 3 + v] = static_cast<float>(vertex[v]);
		}
	}

	glGenBuffers(1, &this->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vertices.size(), vertex_array, GL_STATIC_DRAW);

	free(vertex_array);
}

void model::create_index_buffer(const std::vector<std::array<int, 3>>& faces)
{
	unsigned int* index_array = reinterpret_cast<unsigned int*>(malloc(sizeof(unsigned int) * 3 * faces.size()));

	for (size_t i = 0; i < faces.size(); ++i)
	{
		auto& face = faces[i];

		for (int v = 0; v < 3; ++v)
		{
			index_array[i * 3 + v] = static_cast<unsigned int>(face[v]);
		}
	}

	glGenBuffers(1, &this->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * faces.size(), index_array, GL_STATIC_DRAW);

	free(index_array);
}

model::~model()
{
	glDeleteBuffers(1, &this->vertex_buffer);
	glDeleteBuffers(1, &this->index_buffer);
}

void model::paint()
{
	glColor3f(1, 1, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_FRONT_AND_BACK);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer);
	glDrawElements(GL_TRIANGLES, this->num_faces * 3, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
}
