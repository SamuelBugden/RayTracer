#include "Vector3D.h"

using namespace std;

Vector3D::Vector3D(double x, double y, double z)
{
	_x = x;
	_y = y;
	_z = z;
}
Vector3D::Vector3D()
{
}
Vector3D::~Vector3D()
{
}
double* Vector3D::GetVar()
{
	double var[3] = { this->_x, this->_y, this->_z };
	return var;
}
double Vector3D::Magnitude()
{
	double mag = sqrt((pow(this->_x, 2.0) + pow(this->_y, 2.0) + pow(this->_z, 2.0)));
	return mag;
}
double Vector3D::DotProduct(const Vector3D& vector)
{
	double dot_prod;
	Vector3D multiplied;
	multiplied._x = this->_x * vector._x;
	multiplied._y = this->_y * vector._y;
	multiplied._z = this->_z * vector._z;
	dot_prod = multiplied._x + multiplied._y + multiplied._z;
	return dot_prod;
}
Vector3D Vector3D::MakeUnitVector()
{
	Vector3D vector(this->_x, this->_y, this->_z);
	Vector3D unit_vector = vector / vector.Magnitude();
	return unit_vector;
}
Vector3D Vector3D::operator+(const Vector3D& vec_to_add)
{
	Vector3D added;
	added._x = this->_x + vec_to_add._x;
	added._y = this->_y + vec_to_add._y;
	added._z = this->_z + vec_to_add._z;
	return added;
}
Vector3D Vector3D::operator-(const Vector3D& vec_to_subtract)
{
	Vector3D subtract;
	subtract._x = this->_x - vec_to_subtract._x;
	subtract._y = this->_y - vec_to_subtract._y;
	subtract._z = this->_z - vec_to_subtract._z;
	return subtract;
}
Vector3D Vector3D::operator*(double scalar)
{
	Vector3D multiply;
	multiply._x = this->_x * scalar;
	multiply._y = this->_y * scalar;
	multiply._z = this->_z * scalar;
	return multiply;
}
Vector3D Vector3D::operator/(double scalar)
{
	Vector3D divide;
	divide._x = this->_x / scalar;
	divide._y = this->_y / scalar;
	divide._z = this->_z / scalar;
	return divide;
}
Vector3D operator*(double scalar, const Vector3D& vector){
	Vector3D multiply;
	multiply._x = vector._x * scalar;
	multiply._y = vector._y * scalar;
	multiply._z = vector._z * scalar;
	return multiply;
}
ostream& operator<<(ostream& os, const Vector3D& vector)
{
	os << "(" << vector._x << ',' << vector._y << ',' << vector._z << ")";
	return os;
}