#pragma once
#include "vec.h"
#include "vertex.h"

class line {
public:
	vec P;
	vec V;
	line(vec& a, vec& b) :P(a), V(b - a) {}
};