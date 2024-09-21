#pragma once
#include <cmath>
#include <algorithm>

class vec {
private:
	double xyz[3];
public:
	vec(){xyz[0] = 0;xyz[1] = 0;xyz[2] = 0;}
	vec(const vec& v){xyz[0] = v.xyz[0];xyz[1] = v.xyz[1];xyz[2] = v.xyz[2];}
	vec(double x, double y, double z) { xyz[0] = x; xyz[1] = y;xyz[2] = z;}
	vec(const double arr[3]) { xyz[0] = arr[0]; xyz[1] = arr[1]; xyz[2] = arr[2];}
	void operator =(const vec& v) {xyz[0] = v.xyz[0]; xyz[1] = v.xyz[1]; xyz[2] = v.xyz[2]; }

	void operator/=(const double scalar) { for (int i = 0; i < 3; i++) xyz[i] /= scalar; }
	void operator*=(const double scalar) { for (int i = 0; i < 3; i++) xyz[i] *= scalar; }
	void operator+=(const vec& v) { for (int i = 0; i < 3; i++) xyz[i] += v.xyz[i]; }
	void operator-=(const vec& v) { for (int i = 0; i < 3; i++) xyz[i] -= v.xyz[i]; }

	vec operator/(const double scalar) const{ return vec(xyz[0] / scalar, xyz[1] / scalar, xyz[2] / scalar); }
	vec operator*(const double scalar) const{ return vec(xyz[0] * scalar, xyz[1] * scalar, xyz[2] * scalar); }
	vec operator+(const vec& v) const{ return vec(xyz[0] + v.xyz[0], xyz[1] + v.xyz[1], xyz[2] + v.xyz[2]); }
	vec operator-(const vec& v) const{ return vec(xyz[0] - v.xyz[0], xyz[1] - v.xyz[1], xyz[2] - v.xyz[2]); }

	double* getComponents() { return xyz; }
	double get(const int i) const { return xyz[i]; }
	double& operator[](const int i) { return xyz[i]; }
	double magnitude() const {
		double mag = 0;
		for (int i = 0; i < 3; i++)
			mag += xyz[i] * xyz[i];
		return sqrt(mag);
	}
	void makeUnit() { *this /= this->magnitude(); }
	vec mp(vec& v) const{ return (*this+v)*0.5; }
	vec cross(const vec& N) const{ 
		return vec(
			xyz[1] * N.xyz[2] - xyz[2] * N.xyz[1],
			xyz[2] * N.xyz[0] - xyz[0] * N.xyz[2],
			xyz[0] * N.xyz[1] - xyz[1] * N.xyz[0]
		);
	}
	vec proj(const vec& b) const{ return (b / b.magnitude() ) * (*this * b); }
	double operator*(const vec& v) const{
		double sum = 0;
		for (int i = 0; i < 3; i++) sum += xyz[i] * v.xyz[i];
		return sum;
	}
	bool approx(const vec& v, double tol) { return (*this - v).magnitude() <= tol; }

	void rot(double x, double y, double z) {
		double x1=0;
		double y1=0;
		if (x != 0) {
			x1= xyz[1];
			y1= xyz[2];
			xyz[1] = x1 * cos(x) - y1 * sin(x);
			xyz[2] = y1 * cos(x) + x1 * sin(x);
		}
		if (y != 0){
			x1 = xyz[2];
			y1 = xyz[0];
			xyz[2] = x1*cos(y) - y1*sin(y);
			xyz[0] = y1*cos(y) + x1*sin(y);
		}
		if (z != 0) {
			x1 = xyz[0];
			y1 = xyz[1];
			xyz[0] = x1*cos(z) - y1*sin(z);
			xyz[1] = y1*cos(z) + x1*sin(z);
		}

		

	}
	friend vec operator*(const double scalar, const vec& v) { return v*scalar; }
};