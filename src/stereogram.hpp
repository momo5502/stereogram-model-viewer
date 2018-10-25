#pragma once

#include <shader.hpp>
#include <paintable.hpp>

//#define SHOW_DEPTH

class stereogram : public paintable
{
public:
	stereogram();
	~stereogram() override;

	void paint() override;

private:
	struct color
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	int width = 0;
	int height = 0;

	int pattern_width = 0;
	const int pattern_div = 12;

	std::unique_ptr<float[]> depth_buffer;
	std::unique_ptr<color[]> color_buffer;
	std::unique_ptr<color[]> pattern;

	GLuint texture = 0;
	std::unique_ptr<shader> shader_program;

	void adjust_buffers();
	void randomize_pattern();
	void prepare_color_buffer();
	void fill_depth_buffer();
	void fill_color_buffer();
	void paint_color_buffer();

	void create_texture();
	void update_texture();

	unsigned int get_depth_value(int x, int y);
	color get_color_value(int x, int y);
	void set_color_value(int x, int y, color value);
};
