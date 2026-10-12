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

	//----------------------------------------------------------Surface properties of whichever object a ray hit

	struct Material{
		double k_a, k_d, k_s, alpha, reflectivity;
		double colour[3];
	};

	BMP _myimage;
	const char* _directory;
	vector <Sphere> _spherelist;
	vector <Plane> _planelist;
	vector <Vector3D> _lightsourcelist;
	Vector3D _camera;
	Viewplane _viewplane;
	int _max_depth;
	double _light_intensity;
public:

	//----------------------------------------------------------Constructors and Destructors

	Scene(Vector3D camera, Viewplane viewplane, Vector3D lightsource);
	~Scene();

	//----------------------------------------------------------Scene Population Functions (eg. Adding multiply spheres)

	void AddSphere(Sphere sphere);
	void AddPlane(Plane plane);
	void AddLightsource(Vector3D lightsource);
	void SetDirectory(const char* directory);
	void SetMaxDepth(int max_depth);
	void SetLightIntensity(double light_intensity);

	//----------------------------------------------------------Scene Object Functions

	bool NearestSurface(Ray ray, Vector3D& point, Vector3D& normal, Material& material);
	bool InShadow(Vector3D point, Vector3D lightsource);
	void Trace(Ray ray, int depth, double colour[3]);

	//----------------------------------------------------------Image Render Functions

	void inputBMP(double illumination[], int i, int j);
	void Engage();
};
#endif
