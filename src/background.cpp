#include "std_include.hpp"

#include "background.hpp"

background::background(float r, float g, float b) : _r(r), _g(g), _b(b)
{

}

void background::paint()
{
	glClearColor(this->_r, this->_g, this->_b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
