#pragma once
#include <vector>
#include "vec.h"
#include "Graphics.h"

using namespace std;
class vertex {
private:
	vec pos;
	vec normal;
	Color c;
public:
	vertex() :pos(1, 0, 0), normal(1,0,0), c(255,255,255){}
	vertex(vec& p, Color co = Color(255, 255, 255)) :pos(p), normal(p), c(co) { normal.makeUnit(); }
	vertex(const vertex& v) :pos(v.pos), normal(v.normal), c(v.c) {}
	double operator[](int i) {return pos[i];}
	void operator=(const vertex& v){ pos = v.pos; normal = v.normal; c = v.c; }
	vec getPos() const{ return pos; }
	vec& getPos() { return pos; }
	vec getNormal() const{ return normal; }
	void setNormal(const vec& n) { normal = n; }
	void setPos(const vec& newPos) { pos = newPos; }
	Color getColor() { return c; }
	void setColor(const Color& co) { c = co; }
	void rot(double x, double y, double z) {
		pos.rot(x, y, z);
		normal.rot(x, y, z);
	}
	void shift(vec newPos){ pos += newPos; }
};