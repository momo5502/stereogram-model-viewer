#pragma once

#include "painter_list.hpp"

class window
{
public:
	window(int width, int height, const std::string& title);
	~window();

	operator GLFWwindow*();

	void show();

	painter_list* get_painter_list();

	bool is_key_pressed(int key);

	long long get_last_frame_time();

private:
	GLFWwindow* handle = nullptr;

	painter_list list;

	long long last_frame_time;
	std::chrono::system_clock::time_point last_frame = std::chrono::system_clock::now();

	void update_frame_times();

	void create(int width, int height, const std::string& title);

	void init_glfw();
	void init_glew();

	void size_callback(int width, int height);
	static void size_callback_static(GLFWwindow* window, int width, int height);
};
