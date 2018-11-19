#include "std_include.hpp"

#include "obj_loader.hpp"

#pragma warning(push)
#pragma warning(disable: 4100)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

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
		tinyobj::MaterialFileReader mtl_reader(this->file_path.parent_path().generic_string() + "/");

		tinyobj::LoadObj(&this->attrib, &this->shapes, &this->materials, nullptr, nullptr, &file, &mtl_reader);

		this->load_textures();
		this->sort_surfaces();
	}
}

void obj_loader::load_textures()
{
	this->textures.clear();
	for (auto& material : this->materials)
	{
		int width = 0, height = 0, bpp = 0;
		auto tex_path = this->file_path.parent_path() / material.ambient_texname;

		uint8_t* rgb_image = stbi_load(tex_path.generic_string().data(), &width, &height, &bpp, 4);
		auto _ = gsl::finally([rgb_image]()
		{
			stbi_image_free(rgb_image);
		});

		model::texture tex;
		tex.width = width;
		tex.height = height;

		tex.data.resize(tex.width * tex.height * 4);
		std::memmove(tex.data.data(), rgb_image, tex.width * tex.height * 4);

		this->textures.push_back(std::move(tex));
	}
}

void obj_loader::sort_surfaces()
{
	this->surfaces.clear();
	this->surfaces.resize(this->materials.size());

	for (auto& shape : this->shapes)
	{
		for (size_t i = 0; i < shape.mesh.material_ids.size(); ++i)
		{
			auto material = shape.mesh.material_ids[i];
			if (material == -1) material = 0;

			this->surfaces[material].indices.push_back(shape.mesh.indices[i * 3 + 0]);
			this->surfaces[material].indices.push_back(shape.mesh.indices[i * 3 + 1]);
			this->surfaces[material].indices.push_back(shape.mesh.indices[i * 3 + 2]);
		}
	}
}

model obj_loader::get_model()
{

	return model(this->attrib, this->surfaces, this->textures);
}
