#ifndef __VECTOR3D
#define __VECTOR3D

#include <iostream>
#include <cmath>
using namespace std;

class Vector3D
{
private:
	double _x, _y, _z;
public:

	//----------------------------------------------------------Constructors and Destructors

	Vector3D(double x, double y, double z);
	Vector3D();
	~Vector3D();

	//----------------------------------------------------------Call Member Functions

	double* GetVar();

	//----------------------------------------------------------Vector Maths Functions 

	double Magnitude();
	double DotProduct(const Vector3D& vector);
	Vector3D MakeUnitVector();

	//----------------------------------------------------------Overloaded Operators

	Vector3D operator+(const Vector3D& vec_to_add);
	Vector3D operator-(const Vector3D& vec_to_subtract);
	Vector3D operator*(double scalar);
	Vector3D operator/(double scalar);
	friend Vector3D operator*(double scalar, const Vector3D& vector);
	friend ostream& operator<<(ostream& os, const Vector3D& vector);
};
#endif