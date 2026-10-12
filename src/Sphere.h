#ifndef __SPHERE
#define __SPHERE

#include <iostream>
#include "Ray.h"
#include "Vector3D.h"

using namespace std;

class Sphere
{
private:
	double _r;
	Vector3D _centre;
	double _k_a, _k_d, _k_s, _alpha;
	int _red, _green, _blue;
	double _reflectivity;
public:

	//----------------------------------------------------------Constructors and Dstructors

	Sphere(double r, Vector3D centre, double k_a, double k_d, double k_s, double alpha, int red, int green, int blue, double reflectivity = 0);
	Sphere();
	~Sphere();

	//----------------------------------------------------------Call Member Functions

	double GetRadius();
	Vector3D GetCentre();

	double GetK_a();
	double GetK_d();
	double GetK_s();
	double GetAlpha();

	int GetRed();
	int GetGreen();
	int GetBlue();
	double GetReflectivity();

	//----------------------------------------------------------Ray-Sphere Calculations

	bool intersect(Ray ray);
	double Distance(Ray ray);
	bool checkShadow(Ray ray,Vector3D lightsource);
	Ray Reflect(Ray ray);

	//----------------------------------------------------------Overloaded Operators

	friend ostream& operator<<(ostream& os, const Sphere& sphere);
};
#endif