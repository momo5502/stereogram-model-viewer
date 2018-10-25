#include "std_include.hpp"

#include "painter_list.hpp"

void painter_list::add(paintable* object)
{
	std::lock_guard<std::recursive_mutex> _(this->mutex);

	this->remove(object);
	this->objects.push_back(object);
}

void painter_list::remove(paintable* object)
{
	std::lock_guard<std::recursive_mutex> _(this->mutex);

	if (this->painting)
	{
		throw std::runtime_error("Modifying the painter list while painting is forbidden");
	}

	auto element = std::find(this->objects.begin(), this->objects.end(), object);

	if (element != this->objects.end())
	{
		this->objects.remove(object);
	}
}

void painter_list::paint()
{
	std::lock_guard<std::recursive_mutex> _(this->mutex);

	this->painting = true;
	auto __ = gsl::finally([this]
	{
		this->painting = false;
	});

	for (auto& object : this->objects)
	{
		object->paint();
	}

}
