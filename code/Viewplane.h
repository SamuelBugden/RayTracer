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
public:

	//----------------------------------------------------------Constructors and Destructors

	Viewplane(int pixelwidth, int pixelheight, double width, double height, Vector3D position);
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
