#include "Ray.h"
using namespace std;

Ray::Ray(Vector3D unit, Vector3D point)
{
	_unit_vector = unit;
	_point = point;
}
Ray::Ray()
{
}
Ray::~Ray()
{
}
Vector3D Ray::GetPoint()
{
	return this->_point;
}
Vector3D Ray::GetUnit()
{
	return this->_unit_vector;
}
ostream& operator<<(ostream& os, const Ray& ray)
{
	os << "Unit vector: " << ray._unit_vector << " --  Point: " << ray._point;
	return os;
}