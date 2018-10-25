#pragma once

#include <paintable.hpp>

class background : public paintable
{
public:
	background(float r, float g, float b);

	void paint() override;

private:
	float _r, _g, _b;
};
