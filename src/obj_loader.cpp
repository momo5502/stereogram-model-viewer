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
		std::string line;
		while (std::getline(file, line))
		{
			this->parse_line(line);
		}
	}
}

void obj_loader::parse_line(std::string line)
{
	if (line.size() <= 2) return;

	std::string command(line.begin(), line.begin() + 2);
	std::string data(line.begin() + 2, line.end());

	if (command == "v ")
	{
		this->parse_vertex(data);
	}
	else if (command == "f ")
	{
		this->parse_face(data);
	}
}

void obj_loader::parse_face(std::string data)
{
	std::array<int, 3> face = { 0,0,0 };

	int index = 0;
	this->temporary_values.clear();
	this->temporary_number.clear();

	for (auto& val : data)
	{
		if (val == ' ')
		{
			index = 0;

			if (!this->temporary_number.empty())
			{
				this->temporary_values.push_back(this->temporary_number);
			}

			this->temporary_number.clear();
		}
		else if (val == '/')
		{
			++index;
		}
		else if (index == 0)
		{
			this->temporary_number.push_back(val);
		}
	}

	if (!this->temporary_number.empty())
	{
		this->temporary_values.push_back(this->temporary_number);
	}

	for (size_t i = 0; i < this->temporary_values.size() && i < 3; ++i)
	{
		face[i] = atoi(this->temporary_values[i].data()) - 1;
	}

	this->faces.push_back(face);

	if (this->temporary_values.size() == 4)
	{
		for (size_t i = 2; i <= this->temporary_values.size(); ++i)
		{
			face[i - 2] = atoi(this->temporary_values[i % this->temporary_values.size()].data()) - 1;
		}

		this->faces.push_back(face);
	}

	if (this->temporary_values.size() > 4)
	{
		OutputDebugStringA("Triangulation needed");
	}
}

void obj_loader::parse_vertex(std::string data)
{
	glm::dvec4 vertex = { 1.0, 1.0, 1.0, 1.0 };

	this->temporary_values.clear();
	this->temporary_number.clear();

	for (auto& val : data)
	{
		if (val == ' ')
		{
			if (!this->temporary_number.empty())
			{
				this->temporary_values.push_back(this->temporary_number);
			}

			this->temporary_number.clear();
		}
		else
		{
			this->temporary_number.push_back(val);
		}
	}

	if (!this->temporary_number.empty())
	{
		this->temporary_values.push_back(this->temporary_number);
	}

	for (size_t i = 0; i < this->temporary_values.size() && i < 4; ++i)
	{
		vertex[glm::dvec4::length_type(i)] = atof(this->temporary_values[i].data());
	}

	this->vertices.push_back(vertex);
}

model obj_loader::get_model()
{
	return model(this->vertices, this->faces);
}
