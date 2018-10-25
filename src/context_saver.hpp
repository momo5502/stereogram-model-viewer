#pragma once

class context_saver
{
public:
	context_saver();
	~context_saver();

private:
	int shader_program;
	int texture_2d;
	int matrix_mode;
	int alignment;

	double modelview_matrix[16];
	double projection_matrix[16];
};
