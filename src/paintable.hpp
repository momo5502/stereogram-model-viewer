#pragma once

class paintable
{
public:
	virtual ~paintable() {}

	virtual void paint() = 0;
};
