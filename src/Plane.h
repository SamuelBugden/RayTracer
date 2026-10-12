#ifndef __PLANE
#define __PLANE

#include "Vector3D.h"
#include "Ray.h"

using namespace std;

class Plane{
private:
	Vector3D _NormalUnitVector;
	Vector3D _PointOnPlane;
	double _k_a, _k_d, _k_s, _alpha;
	int _red, _green, _blue;
	double _reflectivity;
	int _check_red, _check_green, _check_blue;
	double _tile_size;
public:

	//----------------------------------------------------------Constructors and Destructors

	Plane(Vector3D normal, Vector3D point, double k_a, double k_d, double k_s, double alpha, int red, int green, int blue, double reflectivity = 0);
	Plane();
	~Plane();

	//----------------------------------------------------------Call Member Functions

	Vector3D GetNormal();
	Vector3D GetPoint();

	double GetK_a();
	double GetK_d();
	double GetK_s();
	double GetAlpha();

	int GetRed();
	int GetGreen();
	int GetBlue();
	double GetReflectivity();

	//----------------------------------------------------------Surface Pattern (eg. a chequered floor)

	void SetChecker(int red, int green, int blue, double tile_size);
	void GetColourAt(Vector3D point, double colour[3]);
	
	//----------------------------------------------------------Ray-Plane Calculations

	bool intersect(Ray ray);
	double Distance(Ray ray);
	bool checkShadow(Ray ray, Vector3D lightsource);
	Ray reflected(Ray ray);
};
#endif