#pragma once

inline auto VoltageDivider = [](float bottom, float top) -> float
{
	return bottom / (bottom + top);
};

inline auto ParallelCircuit = [](float a, float b) -> float
{
	return (a * b) / (a + b);
};

inline auto InvertingAmpWithBias = [](float in, float r_in, float r_feedback, float bias) -> float
{
	return bias - r_feedback / r_in * (in - bias);
};
