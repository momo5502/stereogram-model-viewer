#pragma once

#include "window.hpp"
#include "paintable.hpp"

class button
{
public:
	button(std::vector<int> _codes) : codes(_codes) {}

	bool is_pressed(window* frame)
	{
		for (auto& code : this->codes)
		{
			if (frame->is_key_pressed(code))
			{
				return true;
			}
		}

		return false;
	}
	
private:
	std::vector<int> codes;
};

class camera : public paintable
{
public:
	camera(window* frame);
	~camera() override;

	void paint() override;

	glm::dvec3 get_position();

private:
	window* frame;

	button key_up;
	button key_down;
	button key_left;
	button key_right;

	double last_x = 0.0;
	double last_y = 0.0;

	glm::dvec3 position = { 0.0, 0.0, 5.0 };
	glm::dvec3 direction = { 0.0, 0.0, -5.0 };
	glm::dvec3 up = { 0.0, 1.0, 0.0 };

	void adjust_position();
	void adjust_angle();

	void transform_world();

	glm::dvec3 calculate_right_movement();
	glm::dvec3 calculate_forward_movement(bool normalize = true);

	double enclose_vertical_angle(double pitch);
};
