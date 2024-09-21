#pragma once
#include <vector>
#include <fstream>
#include "vertex.h"
#include "vec.h"
class triangle {
public:
	vertex* verts[3];
	vec normal;
	Color c;
	triangle(vertex& a, vertex& b, vertex& c) :
		normal(),
		c(
			(a.getColor().GetR() + b.getColor().GetR() + c.getColor().GetR()) / 3,
			(a.getColor().GetG() + b.getColor().GetG() + c.getColor().GetG()) / 3,
			(a.getColor().GetB() + b.getColor().GetB() + c.getColor().GetB()) / 3
		)
	{
		verts[0] = &a;
		verts[1] = &b; 
		verts[2] = &c;
		vec cross((a.getPos() - b.getPos()).cross(c.getPos() - b.getPos()));
		normal = cross * a.getPos() > 0 ? cross : -1 * cross;
		normal.makeUnit();
	}
	void updateNormal() {
		vec cross((verts[0]->getPos() - verts[1]->getPos()).cross(verts[2]->getPos() - verts[1]->getPos()));
		normal = cross * verts[0]->getPos() > 0 ? cross : -1 * cross;
		normal.makeUnit();
	}
	void updateColor() {
		c = Color(
			(verts[0]->getColor().GetR() + verts[1]->getColor().GetR() + verts[2]->getColor().GetR()) / 3,
			(verts[0]->getColor().GetG() + verts[1]->getColor().GetG() + verts[2]->getColor().GetG()) / 3,
			(verts[0]->getColor().GetB() + verts[1]->getColor().GetB() + verts[2]->getColor().GetB()) / 3
			);
	}

};
class geometry {
public:
	vector<triangle>faces;
	vector<vertex> vertices;
	vec root;
	double backFaceLimt = 0.5;
	int vertCount=0;
	geometry():root(0,0,0),vertices(), vertCount(0) {}
	geometry(ifstream& f):root(0,0,0) {
		char c;
		int size;
		f >> size;
		vertCount = size;
		for (int i = 0; i < vertCount;i++) {
			double x, y, z;
			int r, b, g;
			f >> x >> y >> z >> r >> g >> b;
			vertices.push_back(vertex(vec(x, y, z), Color(r, g, b)));
		}
		while (!f.eof()) {
			int a, b, c;
			f >> a >> b >> c;
			faces.push_back(triangle(vertices[a-1], vertices[b-1], vertices[c-1]));
		}
		/*
		f >> c;
		while (!f.eof()) {
			int i;
			double x, y, z;
			f >> i >> x >> y >> z;
			faces[i].normal=vec(x, y, z);

		}
		*/
		
	}
	geometry(const geometry& g) :root(g.root), faces(g.faces), vertices(g.vertices),vertCount(g.vertCount){ }
	void scale(double scale) { for (int i = 0; i < vertCount;i++)vertices[i].getPos() *= scale; }
	void scale(double x, double y, double z) {
		for (int i = 0; i < vertCount; i++) {
			vertices[i].getPos()[0] = vertices[i].getPos()[0] * x;
			vertices[i].getPos()[1] = vertices[i].getPos()[1] * z;
			vertices[i].getPos()[2] = vertices[i].getPos()[2] * y;
		}
		for (int i = 0; i < faces.size(); i++)
			faces[i].updateNormal();
	}
	void translate(vec v) {root+=v;}
	void setPos(vec v) {root = v;}
	void setPos(double x, double y, double z) { root[0] = x; root[1] = y; root[2] = z; }
	vec getAbsPos(int i) const{ return vertices[i].getPos() + root; }
	void rot(double x, double y, double z) {
		for (int i = 0; i < vertCount; i++)
			vertices[i].rot(x, y, z);
		for (int i = 0; i < faces.size(); i++)
			faces[i].updateNormal();
	}
	void setColor(int i, Color c) {
		vertices[i].setColor(c);
	}
	void setColor(Color c) {
		for (int i = 0; i < vertCount; i++)
			vertices[i].setColor(c);
		for (int i = 0; i < faces.size(); i++)
			faces[i].updateColor();
	}
	void updateColor() {
		for (int i = 0; i < faces.size(); i++)
			faces[i].updateColor();

	}
	vertex& getVert(int a) { return vertices[a]; }
};