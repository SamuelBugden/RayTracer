#include "Plane.h"

Plane::Plane(Vector3D normal, Vector3D point, double k_a, double k_d, double k_s, double alpha, int red, int green, int blue, double reflectivity)
{
	_reflectivity = reflectivity;
	_tile_size = 0;
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
	_tile_size = 0;
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
double Plane::GetReflectivity()
{
	return this->_reflectivity;
}
void Plane::SetChecker(int red, int green, int blue, double tile_size)
{
	_check_red = red;
	_check_green = green;
	_check_blue = blue;
	_tile_size = tile_size;
}
void Plane::GetColourAt(Vector3D point, double colour[3])
{
	colour[0] = _red;
	colour[1] = _green;
	colour[2] = _blue;
	if (_tile_size <= 0)
		return;
	// Build two axes lying in the plane and count whole tiles along each
	Vector3D helper = fabs(_NormalUnitVector.GetX()) < 0.9 ? Vector3D(1, 0, 0) : Vector3D(0, 1, 0);
	Vector3D axis_u = _NormalUnitVector.CrossProduct(helper).MakeUnitVector();
	Vector3D axis_v = _NormalUnitVector.CrossProduct(axis_u);
	Vector3D offset = point - _PointOnPlane;
	long tile_u = (long)floor(offset.DotProduct(axis_u) / _tile_size);
	long tile_v = (long)floor(offset.DotProduct(axis_v) / _tile_size);
	if ((tile_u + tile_v) % 2 != 0){
		colour[0] = _check_red;
		colour[1] = _check_green;
		colour[2] = _check_blue;
	}
}
// Distance along the ray to the plane, or -1 if the ray is parallel to it or points away from it
double Plane::Distance(Ray ray)
{
	double denominator = ray.GetUnit().DotProduct(this->_NormalUnitVector);
	if (fabs(denominator) < 1e-12)
		return -1;
	double lambda = (this->_PointOnPlane - ray.GetPoint()).DotProduct(this->_NormalUnitVector) / denominator;
	if (lambda > 0)
		return lambda;
	return -1;
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