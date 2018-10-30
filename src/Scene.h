#ifndef __SCENE
#define __SCENE

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Ray.h"
#include "Plane.h"
#include "Vector3D.h"
#include "Viewplane.h"
#include "Sphere.h"
#include "EasyBMP.h"

using namespace std;

class Scene{
private:
	BMP _myimage;
	const char* _directory;
	vector <Sphere> _spherelist;
	vector <Plane> _planelist;
	vector <Vector3D> _lightsourcelist;
	vector<Ray> _raylist;
	vector<int> _indexlist;
	vector<int>_objecttype;
	Vector3D _camera;
	Viewplane _viewplane;
public:

	//----------------------------------------------------------Constructors and Destructors

	Scene(Vector3D camera, Viewplane viewplane, Vector3D lightsource);
	~Scene();

	//----------------------------------------------------------Scene Population Functions (eg. Adding multiply spheres)

	void AddSphere(Sphere sphere);
	void AddPlane(Plane plane);
	void AddLightsource(Vector3D lightsource);
	void SetDirectory(const char* directory);

	//----------------------------------------------------------Scene Object Functions

	int CalcNearestSurface(vector<Ray> raylist);
	double* normalisedIllumination(Sphere this_sphere, double ambient, double diffusive, double specular);
	double* normalisedIllumination(Plane this_plane, double ambient, double diffusive, double specular);

	//----------------------------------------------------------Image Render Functions

	void inputBMP(double illumination[], int i, int j);
	void Engage();
};
#endif