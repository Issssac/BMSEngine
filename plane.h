#pragma once
#include "vec.h"

class plane {
private:
	vec N;
	vec A;
	vec B;
	vec P;
	double Amag=0.45;
	double Bmag=0.8;
	double TOLERANCE = 0.00001;
public:
	plane():N(1,0,0),A(),B(),P(){}
	plane(vec& p, vec& a, vec& b) :P(p), A(a), B(b), N(a.cross(b)) { N.makeUnit(); Amag = A.magnitude(); Bmag = B.magnitude(); }

	void scaleA(double scalar) { A *= scalar; Amag = A.magnitude();}
	void scaleB(double scalar) { B *= scalar; Bmag = B.magnitude();}
	void scale(double scalar) { A *= scalar; B *= scalar; Amag = A.magnitude(); Bmag = B.magnitude();}
	void setPos(const vec& p) {P = p;}
	void setA(const vec& a) { A = a; Amag = A.magnitude(); }
	void setB(const vec& b) { B = b; Bmag = B.magnitude(); }
	void setA(double x, double y, double z) {setA(vec(x, y, z));}
	void setB(double x, double y, double z) {setB(vec(x, y, z));}
	void translate(const vec& t) {P += t;}
	void translate(double x, double y, double z) { P += vec(x,y,z); }
	void face(const vec& n) { 
		vec temp = n.cross(B); 
		temp.makeUnit(); 
		temp *= Amag; 
		A = temp; 
		temp = n.cross(A);  
		temp.makeUnit(); 
		temp *= Bmag; 
		N = n / n.magnitude() * N.magnitude(); 
	}
	vec poi(const vec& a, const vec& b) const{
		vec delta(a - b);
		//if (delta.magnitude() > 5)throw false;
		vec offset(P - b);
		if (delta * offset < 0.05)throw false;
		vec sol = delta*(pow(offset.magnitude(),2)/(offset*delta));
		sol -= offset;
		double u = sol*A/Amag;
		double v = sol*B/Bmag;
		return vec(u,v, delta.magnitude());
	}
};