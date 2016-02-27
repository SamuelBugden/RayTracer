#include "Sphere.h"
using namespace std;

Sphere::Sphere(double r, Vector3D centre, double k_a, double k_d, double k_s, double alpha, int red, int green, int blue)
{
	_r = r;
	_centre = centre;
	_k_a = k_a;
	_k_d = k_d;
	_k_s = k_s;
	_alpha = alpha;
	_red = red;
	_green = green;
	_blue = blue;
}
Sphere::Sphere()
{
}
Sphere::~Sphere()
{
}
double Sphere::GetRadius()
{
	double radius = this->_r;
	return radius;
}
Vector3D Sphere::GetCentre()
{
	Vector3D centre = this->_centre;
	return centre;
}
double Sphere::GetK_a()
{
	double x = this->_k_a;
	return x;
}
double Sphere::GetK_d()
{
	double x = this->_k_d;
	return x;
}
double Sphere::GetK_s()
{
	double x = this->_k_s;
	return x;
}
double Sphere::GetAlpha()
{
	double x = this->_alpha;
	return x;
}
int Sphere::GetRed()
{
	int red = this->_red;
	return red;
}
int Sphere::GetGreen()
{
	int green = this->_green;
	return green;
}
int Sphere::GetBlue()
{
	int blue = this->_blue;
	return blue;
}
bool Sphere::intersect(Ray ray)
{
	double a = (ray.GetUnit()).DotProduct(ray.GetUnit());
	double b = (2 * ray.GetUnit()).DotProduct((ray.GetPoint() - this->_centre));
	double c = (ray.GetPoint() - this->_centre).DotProduct(ray.GetPoint() - this->_centre) - pow(this->_r, 2);
	double discriminant = pow(b, 2) - 4 * a * c;
	if (discriminant >= 0)
		return true;
	else
		return false;
}
bool Sphere::checkShadow(Ray ray,Vector3D lightsource)
{
	double a = (ray.GetUnit()).DotProduct(ray.GetUnit());
	double b = (2 * ray.GetUnit()).DotProduct((ray.GetPoint() - this->_centre));
	double c = (ray.GetPoint() - this->_centre).DotProduct(ray.GetPoint() - this->_centre) - pow(this->_r, 2);
	double root1 = (-b + sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
	double root2 = (-b - sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
	if (root1 > 0 && root2 > 0)
		return true;
	else
		return false;
}
Ray Sphere::Reflect(Ray ray)
{
	if (intersect(ray)){
		double lambda;
		double a = (ray.GetUnit()).DotProduct(ray.GetUnit());
		double b = (2 * ray.GetUnit()).DotProduct(ray.GetPoint() - this->_centre);
		double c = (ray.GetPoint() - this->_centre).DotProduct(ray.GetPoint() - this->_centre) - pow(this->_r, 2);
		double discriminant = pow(b, 2) - 4 * a * c;
		//---------------------------------------------------------------------One Possible Root
		if (discriminant == 0){
			double root = (-b + sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
			if (root > 0)
				lambda = root;
			else
				lambda = 0;
		}
		//---------------------------------------------------------------------Two Possible Roots
		else{
			double root1 = (-b + sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
			double root2 = (-b - sqrt(pow(b, 2) - 4 * a*c)) / (2 * a);
			if (root1 > 0 && root2 > 0){
				if (root1 > root2)
					lambda = root2;
				else
					lambda = root1;
			}
			else if (root1 > 0 || root2 > 0){
				if (root1 > 0)
					lambda = root1;
				else
					lambda = root2;
			}
			else
				lambda = 0;
		}
		Vector3D point_of_intersect = (lambda * ray.GetUnit()) + ray.GetPoint();
		Vector3D normal_at_point = (point_of_intersect - this->_centre).MakeUnitVector();
		Vector3D reflected_unit_vector = (ray.GetUnit()) - (2 * ((ray.GetUnit()).DotProduct(normal_at_point))) * normal_at_point;
		Ray reflected(reflected_unit_vector, point_of_intersect);
		return reflected;
	}
	else
		return ray;
}
ostream& operator<<(ostream& os, const Sphere& sphere)
{
	os << "Radius: " << sphere._r << " -- Centre: " << sphere._centre;
	return os;
}