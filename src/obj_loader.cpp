#include "std_include.hpp"

#include "obj_loader.hpp"

obj_loader::obj_loader(std::string path) : file_path(path)
{
	this->parse_file();
}

obj_loader::~obj_loader()
{

}

void obj_loader::parse_file()
{
	if (this->file_path.empty()) return;

	std::ifstream file(this->file_path);

	if (file.good())
	{
		tinyobj::callback_t cb;
		cb.vertex_cb = obj_loader::vertex_callback_static;
		cb.index_cb = obj_loader::index_callback_static;

		tinyobj::LoadObjWithCallback(file, cb, this, nullptr, nullptr, nullptr);
	}
}

model obj_loader::get_model()
{
	return model(this->vertices, this->faces);
}

void obj_loader::vertex_callback(tinyobj::real_t x, tinyobj::real_t y, tinyobj::real_t z, tinyobj::real_t w)
{
	this->vertices.push_back({ x, y, z, w });
}

void obj_loader::vertex_callback_static(void *user_data, tinyobj::real_t x, tinyobj::real_t y, tinyobj::real_t z, tinyobj::real_t w)
{
	reinterpret_cast<obj_loader*>(user_data)->vertex_callback(x, y, z, w);
}

void obj_loader::index_callback(tinyobj::index_t *indices, int num_indices)
{
	if (num_indices >= 3)
	{
		this->faces.push_back({ indices[0].vertex_index - 1, indices[1].vertex_index - 1, indices[2].vertex_index - 1 });

		if (num_indices == 4)
		{
			this->faces.push_back({ indices[2].vertex_index - 1, indices[3].vertex_index - 1, indices[0].vertex_index - 1 });
		}
	}
}

void obj_loader::index_callback_static(void *user_data, tinyobj::index_t *indices, int num_indices)
{
	reinterpret_cast<obj_loader*>(user_data)->index_callback(indices, num_indices);
}
