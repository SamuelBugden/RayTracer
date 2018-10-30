#ifndef __RAY
#define __RAY

#include "Vector3D.h"
#include <iostream>
using namespace std;

class Ray{
private:
	Vector3D _unit_vector, _point;
public:

	//----------------------------------------------------------Constructors and Destructors

	Ray(Vector3D unit, Vector3D point);
	Ray();
	~Ray();

	//----------------------------------------------------------Call Member Functions

	Vector3D GetPoint();
	Vector3D GetUnit();

	//----------------------------------------------------------Overloaded Operators

	friend ostream& operator<<(ostream& os, const Ray& ray);
};
#endif 