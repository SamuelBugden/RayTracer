#include "Viewplane.h"
using namespace std;

Viewplane::Viewplane(int pixelwidth, int pixelheight, double width, double height, Vector3D position)
{
	_pixels_y = pixelheight;
	_pixels_x = pixelwidth;
	_width = width;
	_height = height;
	_position = position;
	_pixelheight = _height / _pixels_y;
	_pixelwidth = _width / _pixels_x;
}
Viewplane::Viewplane()
{
}
Viewplane::~Viewplane()
{
}
double Viewplane::getPixelwidth()
{
	return _pixelwidth;
}
double Viewplane::getPixelheight()
{
	return _pixelheight;
}
int Viewplane::GetPixel_x()
{
	return _pixels_x;
}
int Viewplane::GetPixel_y()
{
	return _pixels_y;
}
Ray Viewplane::GetPixelRay(Vector3D camera, int i, int j)
{
	double x = this->_position.GetVar()[0] - ((this->_width) / 2) + (_pixelwidth / 2) + (i * _pixelwidth);
	double y = this->_position.GetVar()[1] + ((this->_height) / 2) - (_pixelheight / 2) - (j * _pixelheight);
	double z = this->_position.GetVar()[2];
	Vector3D pixel(x, y, z);
	Vector3D vector_to_pixel = pixel - camera;
	Vector3D unit_vector = vector_to_pixel.MakeUnitVector();
	Ray PixelRay(unit_vector, camera);
	return PixelRay;
}