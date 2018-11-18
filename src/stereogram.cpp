#include "std_include.hpp"

#include "random.hpp"
#include "stereogram.hpp"
#include "context_saver.hpp"

stereogram::stereogram()
{
	static_assert(sizeof(stereogram::color) == 3);

	static auto vertex_shader_source =
		"void main(void)"
		"{"
		"	gl_TexCoord[0] = gl_MultiTexCoord0;"
		"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
		"}";

	static auto fragment_shader_source =
		"uniform sampler2D tex_sampler;"
		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(tex_sampler,gl_TexCoord[0].st);"
		//"   gl_FragColor.a = 0.5;"
		"}";

	this->shader_program = std::make_unique<shader>(vertex_shader_source, fragment_shader_source);
}

stereogram::~stereogram()
{

}

void stereogram::paint()
{
	glFlush();

	this->adjust_buffers();
	this->fill_depth_buffer();
	this->fill_color_buffer();
	this->paint_color_buffer();
}

void stereogram::adjust_buffers()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int viewport_width = viewport[2] - viewport[0];
	int viewport_height = viewport[3] - viewport[1];

	if (!this->texture || !this->depth_buffer || !this->pattern || viewport_width != this->width || viewport_height != this->height)
	{
		this->width = viewport_width;
		this->height = viewport_height;
		this->depth_buffer.reset(new float[this->width * this->height]);
		this->color_buffer.reset(new stereogram::color[this->width * this->height]);

		this->pattern_width = static_cast<int>((this->width * 1.0) / this->pattern_div);
		this->pattern.reset(new stereogram::color[this->pattern_width * height]);

		this->randomize_pattern();
		this->create_texture();
	}
	else
	{
		this->randomize_pattern();
	}
}

void stereogram::create_texture()
{
	context_saver _;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->color_buffer.get());
}

void stereogram::update_texture()
{
	GLint texture_2d;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture_2d);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	if (GetKeyState(VK_CAPITAL) & 0x0001) // Ugly, but for now it's ok
	{
		for (int y = 0; y < this->height; ++y)
		{
			for (int x = 0; x < this->width; ++x)
			{
				unsigned char depth_value = static_cast<unsigned char>(this->get_depth_value(x, y));

				color color;
				color.r = depth_value;
				color.g = depth_value;
				color.b = depth_value;
				this->set_color_value(x, y, color);
			}
		}
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, this->color_buffer.get());

	glBindTexture(GL_TEXTURE_2D, texture_2d);
}

void stereogram::randomize_pattern()
{
	for (int i = 0; i < this->pattern_width * this->height; ++i)
	{
		stereogram::color value;

		for (int c = 0; c < 3; ++c)
		{
			(&value.r)[c] = static_cast<unsigned char>(random::fastrand());
		}

		pattern[i] = value;
	}

	this->prepare_color_buffer();
}

void stereogram::fill_depth_buffer()
{
	if (this->depth_buffer)
	{
		glReadPixels(0, 0, this->width, this->height, GL_DEPTH_COMPONENT, GL_FLOAT, this->depth_buffer.get());
	}
}

void stereogram::prepare_color_buffer()
{
	const auto row_length = this->pattern_width * sizeof(stereogram::color);

	for (int y = 0; y < this->height; ++y)
	{
		auto color_offset = y * this->width;
		auto pattern_offset = y * this->pattern_width;

		std::memmove(this->color_buffer.get() + color_offset, this->pattern.get() + pattern_offset, row_length);
	}
}

void stereogram::fill_color_buffer()
{
	for (int y = 0; y < this->height; ++y)
	{
		for (int x = this->pattern_width; x < this->width; ++x)
		{
			auto depth_value = this->get_depth_value(x, y);

			auto shift = depth_value / this->pattern_div;
			auto x_translate = x - this->pattern_width + shift;

			while (x_translate < 0) x_translate += this->pattern_width;

			auto color = this->get_color_value(x_translate, y);
			this->set_color_value(x, y, color);
		}
	}

	this->update_texture();
}

void stereogram::paint_color_buffer()
{
	context_saver _;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0.0, this->width * 1.0, 0.0, this->height * 1.0, -1.0, 1.0);

	this->shader_program->use();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4i(255, 255, 255, 255);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glBegin(GL_QUADS);
	int x = 0, y = 0;
	glTexCoord2i(0, 0); glVertex3i(x, y, 0);
	glTexCoord2i(0, 1); glVertex3i(x, (this->height + y), 0);
	glTexCoord2i(1, 1); glVertex3i((this->width + x), (this->height + y), 0);
	glTexCoord2i(1, 0); glVertex3i((this->width + x), y, 0);
	glEnd();

	glEnable(GL_TEXTURE_2D);
}

unsigned int stereogram::get_depth_value(int x, int y)
{
	double val = this->depth_buffer[x + y * this->width];
	val = 1.0 - val;
	val *= 255;
	return static_cast<unsigned int>(val);
}

stereogram::color stereogram::get_color_value(int x, int y)
{
	return this->color_buffer[x + y * this->width];
}

void stereogram::set_color_value(int x, int y, stereogram::color value)
{
	this->color_buffer[x + y * this->width] = value;
}
