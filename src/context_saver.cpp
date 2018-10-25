#include "std_include.hpp"

#include "context_saver.hpp"

context_saver::context_saver()
{
	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glGetIntegerv(GL_UNPACK_ALIGNMENT, &this->alignment);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &this->texture_2d);
	glGetIntegerv(GL_MATRIX_MODE, &this->matrix_mode);
	glGetDoublev(GL_MODELVIEW_MATRIX, this->modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, this->projection_matrix);
	glGetIntegerv(GL_CURRENT_PROGRAM, &this->shader_program);
}

context_saver::~context_saver()
{
	glUseProgram(this->shader_program);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(this->projection_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(this->modelview_matrix);
	glMatrixMode(this->matrix_mode);
	glBindTexture(GL_TEXTURE_2D, this->texture_2d);
	glPixelStorei(GL_UNPACK_ALIGNMENT, this->alignment);

	glPopAttrib();
	glPopClientAttrib();
}
