#pragma once


class EdgeDetector
{
public:
	EdgeDetector() : val(false) {}

	bool operator()(bool in)
	{
		auto result = not val and in;
		val = in;
		return result;
	}

private:
	bool val;
};

class FallingEdgeDetector
{
public:
	FallingEdgeDetector() : val(false) {}

	bool operator()(bool in)
	{
		auto result = val and not in;
		val = in;
		return result;
	}

private:
	bool val;
};