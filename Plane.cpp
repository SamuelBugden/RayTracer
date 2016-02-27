#include "Plane.h"

Plane::Plane(Vector3D normal, Vector3D point, double k_a, double k_d, double k_s, double alpha, int red, int green, int blue)
{
	_NormalUnitVector = normal.MakeUnitVector();
	_PointOnPlane = point;
	_k_a = k_a;
	_k_d = k_d;
	_k_s = k_s;
	_alpha = alpha;
	_red = red;
	_green = green;
	_blue = blue;
}
Plane::Plane()
{
}
Plane::~Plane()
{
}
Vector3D Plane::GetNormal()
{
	return this->_NormalUnitVector;
}
Vector3D Plane::GetPoint()
{
	return this->_PointOnPlane;
}
double Plane::GetK_a()
{
	double x = this->_k_a;
	return x;
}
double Plane::GetK_d()
{
	double x = this->_k_d;
	return x;
}
double Plane::GetK_s()
{
	double x = this->_k_s;
	return x;
}
double Plane::GetAlpha()
{
	double x = this->_alpha;
	return x;
}
int Plane::GetRed()
{
	int red = this->_red;
	return red;
}
int Plane::GetGreen()
{
	int green = this->_green;
	return green;
}
int Plane::GetBlue()
{
	int blue = this->_blue;
	return blue;
}
bool Plane::intersect(Ray ray)
{
	double d = -(this->_PointOnPlane.DotProduct(this->_NormalUnitVector));
	double lambda = -((ray.GetPoint().DotProduct(this->_NormalUnitVector) + (d)) / (ray.GetUnit().DotProduct(this->_NormalUnitVector)));
	if (lambda > 0)
		return true;
	else
		return false;
}
bool Plane::checkShadow(Ray ray,Vector3D lightsource)
{
	double d = -(this->_PointOnPlane.DotProduct(this->_NormalUnitVector));
	double lambda = -((ray.GetPoint().DotProduct(this->_NormalUnitVector) + (d)) / (ray.GetUnit().DotProduct(this->_NormalUnitVector)));
	double distance_to_intersect = (ray.GetPoint() + (lambda*ray.GetUnit())).Magnitude();
	double distance_to_light = (lightsource - ray.GetPoint()).Magnitude();
	if (lambda > 0){
		if (distance_to_light < distance_to_intersect)
			return false;
		else
			return true;
	}
	else
		return false;
}
Ray Plane::reflected(Ray ray)
{
	if (intersect(ray)){
		double d = (this->_PointOnPlane - ray.GetPoint()).DotProduct(this->_NormalUnitVector) / (ray.GetUnit().DotProduct(this->_NormalUnitVector));
		Vector3D point_of_intersect = d*ray.GetUnit() + ray.GetPoint();
		Vector3D normal_at_point = this->_NormalUnitVector.MakeUnitVector();
		Vector3D reflected_unit_vector = (ray.GetUnit()) - (2 * ((ray.GetUnit()).DotProduct(normal_at_point))) * normal_at_point;
		Ray reflected(reflected_unit_vector, point_of_intersect);
		return reflected;
	}
	else
		return ray;
}