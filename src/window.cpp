#include "std_include.hpp"

#include "window.hpp"

window::window(int width, int height, const std::string& title)
{
	this->init_glfw();
	this->create(width, height, title);
	this->init_glew();
}

window::~window()
{
	glfwTerminate();
}

window::operator GLFWwindow*()
{
	return this->handle;
}

void window::init_glfw()
{
	if (glfwInit() != GLFW_TRUE)
	{
		throw std::runtime_error("Unable to initialize glfw");
	}
}

void window::init_glew()
{
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("Unable to initialize glew");
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void window::create(int width, int height, const std::string& title)
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);

	this->handle = glfwCreateWindow(width, height, title.data(), NULL, NULL);
	if (!this->handle)
	{
		throw std::runtime_error("Unable to create window");
	}

	glfwSetWindowUserPointer(this->handle, this);
	glfwMakeContextCurrent(this->handle);
	glfwSetWindowSizeCallback(this->handle, window::size_callback_static);

	glfwSwapInterval(0);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearDepth(1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_POINT_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
}

void window::size_callback(int width, int height)
{
	glViewport(0, 0, width, height);
}

void window::size_callback_static(GLFWwindow* _window, int width, int height)
{
	reinterpret_cast<window*>(glfwGetWindowUserPointer(_window))->size_callback(width, height);
}

void window::show()
{
	while (this->handle && !glfwWindowShouldClose(this->handle))
	{
		this->update_frame_times();
		this->list.paint();

		glfwSwapBuffers(this->handle);
		glfwPollEvents();
	}
}

painter_list* window::get_painter_list()
{
	return &this->list;
}

bool window::is_key_pressed(int key)
{
	return glfwGetKey(*this, key) == GLFW_PRESS;
}

long long window::get_last_frame_time()
{
	return this->last_frame_time;
}

void window::update_frame_times()
{
	auto now = std::chrono::system_clock::now();
	this->last_frame_time = std::chrono::duration_cast<std::chrono::microseconds>(now - this->last_frame).count();
	this->last_frame = now;
}
