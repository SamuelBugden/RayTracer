#ifndef __VIEWPLANE
#define __VIEWPLANE

#include "Vector3D.h"
#include "Ray.h"
using namespace std;

class Viewplane{
private:
	int _pixels_x, _pixels_y;
	double _width, _height;
	double _pixelwidth, _pixelheight;
	Vector3D _position;
	Vector3D _right, _up;
public:

	//----------------------------------------------------------Constructors and Destructors

	Viewplane(int pixelwidth, int pixelheight, double width, double height, Vector3D position);
	Viewplane(int pixelwidth, int pixelheight, double fov_degrees, Vector3D camera, Vector3D look_at, Vector3D up = Vector3D(0, 1, 0));
	Viewplane();
	~Viewplane();

	//----------------------------------------------------------Call Member Functions

	double getPixelwidth();
	double getPixelheight();

	int GetPixel_x();
	int GetPixel_y();

	//----------------------------------------------------------Ray Calculation (from Cammera position to pixel position)

	Ray GetPixelRay(Vector3D camera, int i, int j);
};
#endif
