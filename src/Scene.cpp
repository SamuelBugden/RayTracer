#include "Scene.h"
#include <iostream>
#include <string>
using namespace std;

// Secondary rays start this far off the surface so they don't immediately re-hit it
static const double EPSILON = 1e-6;

Scene::Scene(Vector3D camera, Viewplane viewplane, Vector3D lightsource)
{
	_camera = camera;
	_lightsourcelist.push_back(lightsource);
	_viewplane = viewplane;
	_directory = "Render.bmp";
	_max_depth = 5;
	_light_intensity = 2.0;
}
Scene::~Scene()
{
}
void Scene::AddSphere(Sphere sphere)
{
	_spherelist.push_back(sphere);
}
void Scene::AddPlane(Plane plane)
{
	_planelist.push_back(plane);
}
void Scene::AddLightsource(Vector3D lightsource)
{
	_lightsourcelist.push_back(lightsource);
}
void Scene::SetDirectory(const char* directory)
{
	_directory = directory;
}
void Scene::SetMaxDepth(int max_depth)
{
	_max_depth = max_depth;
}
void Scene::SetLightIntensity(double light_intensity)
{
	_light_intensity = light_intensity;
}
bool Scene::NearestSurface(Ray ray, Vector3D& point, Vector3D& normal, Material& material)
{
	double nearest = -1;
	int index = -1;
	bool is_sphere = false;
	for (int k = 0; k < (int)_spherelist.size(); k++){
		double distance = _spherelist[k].Distance(ray);
		if (distance > 0 && (nearest < 0 || distance < nearest)){
			nearest = distance;
			index = k;
			is_sphere = true;
		}
	}
	for (int p = 0; p < (int)_planelist.size(); p++){
		double distance = _planelist[p].Distance(ray);
		if (distance > 0 && (nearest < 0 || distance < nearest)){
			nearest = distance;
			index = p;
			is_sphere = false;
		}
	}
	if (index < 0)
		return false;

	point = (nearest * ray.GetUnit()) + ray.GetPoint();
	if (is_sphere){
		Sphere& sphere = _spherelist[index];
		normal = (point - sphere.GetCentre()).MakeUnitVector();
		material = { sphere.GetK_a(), sphere.GetK_d(), sphere.GetK_s(), sphere.GetAlpha(), sphere.GetReflectivity(),
			{ (double)sphere.GetRed(), (double)sphere.GetGreen(), (double)sphere.GetBlue() } };
	}
	else{
		Plane& plane = _planelist[index];
		normal = plane.GetNormal();
		material = { plane.GetK_a(), plane.GetK_d(), plane.GetK_s(), plane.GetAlpha(), plane.GetReflectivity(),
			{ (double)plane.GetRed(), (double)plane.GetGreen(), (double)plane.GetBlue() } };
	}
	// Shade the side of the surface the ray arrived from
	if (normal.DotProduct(ray.GetUnit()) > 0)
		normal = -1 * normal;
	return true;
}
bool Scene::InShadow(Vector3D point, Vector3D lightsource)
{
	Vector3D to_light = lightsource - point;
	double distance_to_light = to_light.Magnitude();
	Ray ray_to_light(to_light.MakeUnitVector(), point);
	// Only spheres cast shadows: the planes form the room's walls and some lights sit outside them
	for (int m = 0; m < (int)_spherelist.size(); m++){
		double distance = _spherelist[m].Distance(ray_to_light);
		if (distance > 0 && distance < distance_to_light)
			return true;
	}
	return false;
}
void Scene::Trace(Ray ray, int depth, double colour[3])
{
	Vector3D point, normal;
	Material material;
	if (!NearestSurface(ray, point, normal, material)){
		colour[0] = colour[1] = colour[2] = 0;
		return;
	}
	Vector3D unit = ray.GetUnit();
	Vector3D reflected_unit = unit - (2 * unit.DotProduct(normal)) * normal;
	Vector3D surface_point = point + EPSILON * normal;

	//------------------------------------------------------Phong shading, with the total light intensity shared between the light sources
	double light_intensity = _light_intensity / _lightsourcelist.size();
	double diffusive = 0, specular = 0;
	for (int l = 0; l < (int)_lightsourcelist.size(); l++){
		if (InShadow(surface_point, _lightsourcelist[l]))
			continue;
		Vector3D unit_to_source = (_lightsourcelist[l] - point).MakeUnitVector();
		diffusive += light_intensity * material.k_d * max(0.0, unit_to_source.DotProduct(normal));
		specular += material.k_s * pow(max(0.0, unit_to_source.DotProduct(reflected_unit)), material.alpha);
	}
	for (int c = 0; c < 3; c++)
		colour[c] = min(255.0, material.colour[c] * (material.k_a + diffusive) + 255 * specular);

	//------------------------------------------------------Reflections: trace the reflected ray and blend it in
	if (material.reflectivity > 0 && depth < _max_depth){
		double reflected_colour[3];
		Trace(Ray(reflected_unit, surface_point), depth + 1, reflected_colour);
		for (int c = 0; c < 3; c++)
			colour[c] = (1 - material.reflectivity) * colour[c] + material.reflectivity * reflected_colour[c];
	}
}
void Scene::inputBMP(double illumination[],int i, int j)
{
	_myimage(i, j)->Red = (int)max(0.0, min(255.0, illumination[0]));
	_myimage(i, j)->Green = (int)max(0.0, min(255.0, illumination[1]));
	_myimage(i, j)->Blue = (int)max(0.0, min(255.0, illumination[2]));
	_myimage(i, j)->Alpha = 0;
}

void Scene::Engage()
{
	int pixel_x = _viewplane.GetPixel_x();
	int pixel_y = _viewplane.GetPixel_y();

	_myimage.SetSize(pixel_x, pixel_y);
	_myimage.SetBitDepth(32);

	cout << "Rendering..." << endl;
	int count_max = pixel_x * pixel_y;
	int count = 0;
	cout << "|< - - -100%- - - >|" << endl;
	for (int j = 0; j < pixel_y; j++){
		for (int i = 0; i < pixel_x; i++){
			count++;
			if (count == (count_max / 10)){
				cout << " #";
				count = 0;
			}
			double illumination[3];
			Trace(_viewplane.GetPixelRay(_camera, i, j), 0, illumination);
			inputBMP(illumination, i, j);
		}
	}
	cout << "\nFinalising.." << endl;
	_myimage.WriteToFile(_directory);
	cout << "Render complete! \nImage Location ->  "<< _directory << endl;
}
