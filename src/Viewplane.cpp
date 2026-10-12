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
	_right = Vector3D(1, 0, 0);
	_up = Vector3D(0, 1, 0);
}
// Viewplane one unit in front of the camera, facing look_at, spanning fov_degrees horizontally
Viewplane::Viewplane(int pixelwidth, int pixelheight, double fov_degrees, Vector3D camera, Vector3D look_at, Vector3D up)
{
	_pixels_y = pixelheight;
	_pixels_x = pixelwidth;
	Vector3D forward = (look_at - camera).MakeUnitVector();
	_right = up.CrossProduct(forward).MakeUnitVector();
	_up = forward.CrossProduct(_right);
	_position = camera + forward;
	_width = 2 * tan(fov_degrees * acos(-1.0) / 360);
	_height = _width * _pixels_y / _pixels_x;
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
	double x = -((this->_width) / 2) + (_pixelwidth / 2) + (i * _pixelwidth);
	double y = ((this->_height) / 2) - (_pixelheight / 2) - (j * _pixelheight);
	Vector3D pixel = this->_position + (x * this->_right) + (y * this->_up);
	Vector3D vector_to_pixel = pixel - camera;
	Vector3D unit_vector = vector_to_pixel.MakeUnitVector();
	Ray PixelRay(unit_vector, camera);
	return PixelRay;
}
