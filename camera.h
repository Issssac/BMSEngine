#pragma once
#include "plane.h"
#include "Graphics.h"
#include "geometry.h"
#include <vector>
#include <thread>
#include <iostream>




struct orderPtr {
	double a[2]{0};
	double b[2]{0};
	double c[2]{0};
	Color color;
};

struct order {
	double dist;
	orderPtr* ord;
	friend bool operator<(order o1, order o2) {
		return o1.dist > o2.dist;
	}
};

class camera {
private:
	vec N;
	vec pos;
	Graphics* gfx;
	plane viewplane;
	int medianScreenW = Graphics::ScreenWidth / 2;
	int medianScreenH = Graphics::ScreenHeight / 2;
	vector<order>orders;
	short charGfx[38][20] = {
		{10,  0,  0, 30,
		 10,  0, 20, 30,
		  5, 15, 15, 15,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//A
		{ 0,  0,  0, 30,
		  0, 15, 17, 15,
		  15, 0, 20, 30,
		  0,  0, 15,  0,
		  0, 30, 20, 30},//B
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 30, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//C
		{ 0,  0,  0, 30,
		  0,  0, 20,  5,
		  0, 30, 20, 25,
		 20,  5, 20, 25,
		  0,  0,  0,  0},//D
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		  0, 30, 20, 30,
		  0,  0,  0,  0},//E
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//F
        { 0,  0,  0, 30,
		  0,  0, 20,  0,
		  7, 15, 20, 15,
		  0, 30, 20, 30,
		 20, 15, 20, 30},//G
		{ 0,  0,  0, 30,
		  0, 15, 20,  15,
		  20, 0, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//H
		{ 0,  0, 20,  0,
		 10,  0, 10, 30,
		 00, 30, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//I
		{ 0, 30,  0, 20,
		 20,  0, 20, 30,
		 00, 30, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//J
		{ 0,  0,  0, 30,
		  0, 15, 20,  0,
		  0, 15, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//K
		{ 0,  0,  0, 30,
		  0, 30, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//L
		{ 0,  0,  0, 30,
		 20,  0, 20, 30,
		  0,  0, 10, 10,
		 10, 10, 20,  0,
		  0,  0,  0,  0},//M
		{ 0,  0,  0, 30,
		 20,  0, 20, 30,
		  0,  0, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//N
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 30, 20, 30,
		 20,  0, 20, 30,
		  0,  0,  0,  0},//O
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		 20,  0, 20, 15,
		  0,  0,  0,  0},//P
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 30, 20, 30,
		 20,  0, 20, 30,
		 15, 26, 22, 32},//Q
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		 20,  0, 20, 15,
		 10, 15, 20, 30},//R
		{ 0,  0, 20,  0,
		  0,  0,  0, 15,
		  0, 15, 20, 15,
		 20, 15, 20, 30,
		  0, 30, 20, 30},//S
		{ 0,  0, 20,  0,
		 10,  0, 10, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//T
		{ 0,  0,  0, 30,
		  0, 30, 20, 30,
		 20, 30, 20,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//U
		{ 0,  0, 10, 30, 
		 20,  0, 10, 30,
		  0,  0,  0,  0,  
		  0,  0,  0,  0,
		  0,  0,  0,  0},//V
		{ 0,  0,  0, 30,
		  0, 30, 10, 23,
		 10, 23, 20, 30,
		 20, 30, 20,  0,
		  0,  0,  0,  0 },//W
		{ 0,  0, 20, 30,
		 20,  0,  0, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0 },//X
		{ 0,  0, 10, 10,
		 10, 10, 20,  0,
		 10, 10, 10, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0 },//Y
		{ 0,  0, 20,  0,
		 20,  0,  0, 30,
		  0, 30, 20, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0},//Z
		{ 0,  0, 20,  0,
		 20,  0, 20, 30,
		 20, 30,  0, 30,
		  0, 30,  0,  0,
		  0,  0, 20, 30 },//0
		{10,  0, 10, 30,
		  0, 30, 20, 30,
		 10,  0,  5,  5,
		  0,  0,  0,  0,
		  0,  0,  0,  0 },//1
		{ 0,  5, 10,  0,
		 10,  0, 20,  5,
		 20,  5,  0, 30,
		  0, 30, 20, 30,
		  0,  0,  0,  0 },//2
		{20,  0, 20, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		  0, 30, 20, 30,
		  0,  0,  0,  0 },//3
		{ 20, 0, 20, 30,
		  0,  0,  0, 15,
		  0, 15, 20, 15,
		  0,  0,  0,  0,
		  0,  0,  0,  0 },//4
		{ 0,  0, 20,  0,
		  0,  0,  0, 10,
		  0, 10, 20, 10,
		 20, 10, 20, 25,
		 20, 25,  0, 30 },//5
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		  0, 30, 20, 30,
		 20, 15, 20, 30 },//6
		{ 0,  0, 20,  0,
		 20,  0,  7, 30,
		  0,  0,  0,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0 },//7
		{ 0,  0,  0, 30,
		  0,  0, 20,  0,
		  0, 15, 20, 15,
		  0, 30, 20, 30,
		 20,  0, 20, 30 },//8
		{ 0,  0, 20,  0,
		 20,  0, 20, 30,
		  0,  0,  0, 15,
		  0, 15, 20, 15,
		  0,  0,  0,  0 },//9
		{10, 30,  5, 25,
		  5, 25, 10, 20,
		 10, 20, 15, 25,
		 15, 25, 10, 30,
		  0,  0,  0,  0 },//.
		{ 0,  0,  0,  0,
		  0, 15, 20, 15,
		  0,  0,  0,  0,
		  0,  0,  0,  0,
		  0,  0,  0,  0 }//-
	};

public:
	bool WF = FALSE;
	camera(Graphics& g, vec& p) :
		gfx(&g), 
		pos(p),
		N(1, 0, 0), 
		viewplane(vec(1,0,0), vec(0, 0, 1.0 / Graphics::ScreenWidth), vec(0, 1.0 / Graphics::ScreenHeight, 0))
	{
	}
	void setPos(double newpos[]) { vec p(newpos); pos = p; viewplane.setPos(pos + N); }
	void translate(double x, double y, double z) { vec v(x, y, z); pos += v; viewplane.translate(v); }
	void translate(const vec& v) { pos += v; viewplane.translate(v); }
	void focal(double x) {N *= x; viewplane.setPos(N + pos);}
	void face(const vec&v) { N = N.magnitude() * v / v.magnitude(); viewplane.setPos(N + pos); viewplane.face(N);}
	vec getNormal(){ return N; }
	void genOrder(triangle& t,geometry& g) {
		vec parr[3];
		for (int i = 0; i < 3; i++) {
			parr[i] = (t.verts[i]->getPos() + g.root);
			if ((parr[i] - pos) * (t.normal) > 0)return;
			try {
				parr[i] = viewplane.poi(parr[i], pos);
			}
			catch (bool) {
				return;
			}
		}
		order o;
		o.ord = new orderPtr;
		o.dist = (parr[0][2] + parr[1][2] + parr[2][2]) / 3;
		o.ord->a[0] = medianScreenW * (1 + parr[0][0]);
		o.ord->a[1] = medianScreenH * (1 - parr[0][1]);
		o.ord->b[0] = medianScreenW * (1 + parr[1][0]);
		o.ord->b[1] = medianScreenH * (1 - parr[1][1]);
		o.ord->c[0] = medianScreenW * (1 + parr[2][0]);
		o.ord->c[1] = medianScreenH * (1 - parr[2][1]);
		o.ord->color = t.c;
		orders.push_back(o);
		return;
	}

	void drawGeo(geometry** g, int numG) {
		for (int GI = 0; GI < numG; GI++) {
			vector <thread> threads;
			for (int TI = 0; TI < g[GI]->faces.size(); TI++) {
				threads.push_back(thread(&camera::genOrder, this, g[GI]->faces[TI], *(g[GI])));
			}
			for (int i = 0; i < threads.size(); i++)
				threads[i].join();
		}

		sort(orders.begin(), orders.end());
		for (int i = 0; i < orders.size(); i++) {
			drawOrder(*(orders[i].ord) );
			delete orders[i].ord;
		}
		orders.clear();
	}
	void CdrawLine(double x1, double y1, double x2, double y2, Color c) {
		double m[2] = { x2 - x1,y2 - y1 };
		double mag = sqrt(m[0] * m[0] + m[1] * m[1]);
		if (mag == 0)
			return;
		//mag /= 1.2;
		m[0] /= mag;
		m[1] /= mag;
		mag += 1;
		for (int i = 0; i < mag; i++) {
			gfx->PutPixel(x1, y1, c);
			x1 += m[0];
			y1 += m[1];
		}

	}
	void drawOrder(orderPtr& o) {
		if (WF) {
			CdrawLine(o.a[0], o.a[1], o.b[0], o.b[1], o.color);
			CdrawLine(o.c[0], o.c[1], o.b[0], o.b[1], o.color);
			CdrawLine(o.a[0], o.a[1], o.c[0], o.c[1], o.color);
			return;
		}
		double m[4] = { o.c[0] - o.a[0],o.c[1] - o.a[1] , o.c[0] - o.b[0], o.c[1] - o.b[1] };

		double mag1 = sqrt(m[0] * m[0] + m[1] * m[1]);
		double mag2 = sqrt(m[2] * m[2] + m[3] * m[3]);
		double mag = mag1 + mag2;
		mag /= 2;
		for (int i = 0; i < 4; i++)
			m[i] /= mag;
		double curr[4] = { o.a[0], o.a[1] , o.b[0], o.b[1] };
		for (int i = 0; i < mag; i++) {
			CdrawLine(curr[0], curr[1], curr[2], curr[3], o.color);
			for (int j = 0; j < 4; j++)curr[j] += m[j];

		}
		return;
	}
	void putChar(char c, int x, int y, Color col=Colors::White , float scale = 1) {
		int C;
		if (c == '-')C = 37;
		else if (c == '.')C = 36;
		else if (isalpha(c))	C = int(c) - int('A');
		else C = 26+int(c) - int('0');
		for (int i = 0; i < 20; i += 4) {
			CdrawLine(charGfx[C][i] * scale+x, charGfx[C][i + 1] * scale +y, charGfx[C][i + 2] * scale +x, charGfx[C][i + 3] * scale +y, col);
		}
	}
	void putStr(string str, int x, int y, float scale = 1, Color col = Colors::White, int space = 30) {
		for (int i = 0; i < str.length(); i++) {
			if (str.at(i) == ' ')continue;
			putChar(str.at(i), x + space * i * scale, y , col, scale);
		}
	}

	void graphArray(float* arr, int length, int x1, int y1, int x2, int y2, float zero, float maxV, Color c) {
		
		float h = y1-y2;
		float w = x2-x1;
		float xRatio = length/w;
		float range = maxV - zero;

		float last = 0;

		for (int i = 0; i < xRatio; i++) last += arr[i];
		last /= floor(xRatio);
		last -= zero;
		last = h * (last / range);
		for (int x = 1; x < w; x++) {
			float yAvg = 0;

			for (int i = 0; i < xRatio; i++) yAvg += arr[ int(x * floor(xRatio) + i) % length];
			yAvg /= floor(xRatio);
			if (yAvg < zero)yAvg = zero;
			yAvg -= zero;
			yAvg = h * (yAvg / range);
			CdrawLine(x1+x-1, min(y1 - max(last,float(0)),float(y1)), x1+x, min( y1 - max(float(0),yAvg),float(y1)), c);
			last = yAvg;

		}

	}
};