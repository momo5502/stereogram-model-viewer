#pragma once

#include "paintable.hpp"

class painter_list
{
public:
	void add(paintable* object);
	void remove(paintable* object);

	void paint();

private:
	bool painting = false;

	std::recursive_mutex mutex;
	std::list<paintable*> objects;
};
