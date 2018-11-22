#include "std_include.hpp"

#include "camera.hpp"

camera::camera(window* _frame) : 
	frame(_frame),
	key_up({ GLFW_KEY_UP,  GLFW_KEY_W }),
	key_down({ GLFW_KEY_DOWN, GLFW_KEY_S }),
	key_left({ GLFW_KEY_LEFT, GLFW_KEY_A }),
	key_right({ GLFW_KEY_RIGHT, GLFW_KEY_D })
{
	glfwSetInputMode(*this->frame, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(*this->frame, &this->last_x, &this->last_y);
}

camera::~camera()
{

}

void camera::paint()
{
	this->adjust_angle();
	this->adjust_position();
	this->transform_world();
}

glm::dvec3 camera::get_position()
{
	return this->position;
}

glm::dvec3 camera::calculate_right_movement()
{
	auto right = glm::cross(this->direction, this->up);
	return glm::normalize(right);
}

glm::dvec3 camera::calculate_forward_movement(bool normalize)
{
	auto forward = glm::dvec3(this->direction[0], 0, this->direction[2]);
	if (normalize) forward = glm::normalize(forward); // Normalization is not always needed
	return forward;
}

double camera::enclose_vertical_angle(double pitch)
{
	static const auto normalize_angle = [](double angle)
	{
		while (angle < -M_PI) angle += M_PI * 2;
		while (angle > M_PI) angle -= M_PI * 2;

		return angle;
	};

	auto forward = this->calculate_forward_movement(false);
	auto angle = std::abs(glm::angle(forward, this->direction));
	angle *= (std::signbit(this->direction[1]) ? -1 : 1);

	const double angle_limit = 80 * (M_PI / 180.0);

	pitch = normalize_angle(pitch);
	pitch = std::min(pitch, normalize_angle(angle_limit - angle));
	pitch = std::max(pitch, normalize_angle(-angle_limit - angle));

	return pitch;
}

void camera::adjust_position()
{
	auto frame_time = this->frame->get_last_frame_time();

	const double speed = 500;
	const double reduction_factor = 0.000001 * speed;
	const double scale = frame_time * reduction_factor;

	if (this->key_up.is_pressed(this->frame))
	{
		this->position += this->direction * scale;
	}
	if (this->key_down.is_pressed(this->frame))
	{
		this->position -= this->direction * scale;
	}

	if (this->key_left.is_pressed(this->frame))
	{
		this->position -= this->calculate_right_movement() * scale;
	}
	if (this->key_right.is_pressed(this->frame))
	{
		this->position += this->calculate_right_movement() * scale;
	}
}

void camera::adjust_angle()
{
	double x, y;
	glfwGetCursorPos(*this->frame, &x, &y);

	double delta_x = this->last_x - x;
	double delta_y = this->last_y - y;

	this->last_x = x;
	this->last_y = y;

	const double speed = 2;
	const double reduction_factor = 0.001 * speed;

	auto pitch = this->enclose_vertical_angle(delta_y * reduction_factor);
	auto yaw = delta_x * reduction_factor;

	auto rotation = glm::rotate(glm::dmat4(1.0), pitch, this->calculate_right_movement());
	rotation = glm::rotate(rotation, yaw, glm::dvec3(0.0, 1.0, 0.0));

	this->direction = glm::dvec3(rotation * glm::dvec4(this->direction, 1.0));
	this->direction = glm::normalize(this->direction);

	//this->up = glm::normalize(glm::dvec3(rotation * glm::dvec4(this->up, 1.0)));
}

void camera::transform_world()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int viewport_width = viewport[2] - viewport[0];
	int viewport_height = viewport[3] - viewport[1];

	gluPerspective(65, viewport_width * 1.0 / viewport_height, 1, 500000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	auto focus_point = this->position + this->direction;

	gluLookAt(this->position[0], this->position[1], this->position[2],
		      focus_point[0]   , focus_point[1]   , focus_point[2],
		      this->up[0]      , this->up[1]      , this->up[2]);
}