#include "Scene.h"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
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
	_primary_rays = _reflection_rays = _shadow_rays = 0;
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
		material = { plane.GetK_a(), plane.GetK_d(), plane.GetK_s(), plane.GetAlpha(), plane.GetReflectivity(), { 0, 0, 0 } };
		plane.GetColourAt(point, material.colour);
	}
	// Shade the side of the surface the ray arrived from
	if (normal.DotProduct(ray.GetUnit()) > 0)
		normal = -1 * normal;
	return true;
}
bool Scene::InShadow(Vector3D point, Vector3D lightsource)
{
	_shadow_rays++;
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
		_reflection_rays++;
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

// Formats a count with thousands separators, eg. 1234567 -> 1,234,567
static string FormatCount(long long n)
{
	string digits = to_string(n);
	string formatted;
	int length = (int)digits.size();
	for (int c = 0; c < length; c++){
		formatted += digits[c];
		if ((length - c - 1) % 3 == 0 && c != length - 1)
			formatted += ',';
	}
	return formatted;
}
static void PrintRule()
{
	cout << "  " << string(60, '-') << "\n";
}
static void PrintField(const string& label, const string& value)
{
	cout << "  " << left << setw(16) << label << value << "\n";
}
void Scene::PrintProgress(int rows_done, int rows_total, double seconds_elapsed)
{
	const int bar_width = 36;
	double fraction = (double)rows_done / rows_total;
	int filled = (int)(fraction * bar_width);

	string bar(filled, '=');
	if (filled < bar_width)
		bar += '>' + string(bar_width - filled - 1, ' ');

	ostringstream timing;
	timing << fixed << setprecision(1);
	if (rows_done < rows_total){
		double remaining = rows_done > 0 ? seconds_elapsed * (rows_total - rows_done) / rows_done : 0;
		timing << "ETA " << remaining << "s";
	}
	else
		timing << "done in " << seconds_elapsed << "s";

	cout << "\r  Rendering  [" << bar << "] " << right << setw(3) << (int)(fraction * 100) << "%  "
		<< left << setw(18) << timing.str() << flush;
}
void Scene::Engage()
{
	int pixel_x = _viewplane.GetPixel_x();
	int pixel_y = _viewplane.GetPixel_y();

	_myimage.SetSize(pixel_x, pixel_y);
	_myimage.SetBitDepth(32);
	_primary_rays = _reflection_rays = _shadow_rays = 0;

	//------------------------------------------------------Scene summary
	cout << "\n  RayTracer\n";
	PrintRule();
	PrintField("Resolution", to_string(pixel_x) + " x " + to_string(pixel_y) + " px");
	PrintField("Objects", to_string(_spherelist.size()) + " spheres, " + to_string(_planelist.size()) + " planes");
	PrintField("Lights", to_string(_lightsourcelist.size()));
	PrintField("Max bounces", to_string(_max_depth));
	PrintRule();

	//------------------------------------------------------Render, updating the progress bar once per row
	auto start = chrono::steady_clock::now();
	int last_percent = 0;
	PrintProgress(0, pixel_y, 0);
	for (int j = 0; j < pixel_y; j++){
		for (int i = 0; i < pixel_x; i++){
			double illumination[3];
			_primary_rays++;
			Trace(_viewplane.GetPixelRay(_camera, i, j), 0, illumination);
			inputBMP(illumination, i, j);
		}
		int percent = (100 * (j + 1)) / pixel_y;
		if (percent != last_percent){
			last_percent = percent;
			double elapsed = chrono::duration<double>(chrono::steady_clock::now() - start).count();
			PrintProgress(j + 1, pixel_y, elapsed);
		}
	}
	double render_seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
	cout << "\n";
	PrintRule();

	//------------------------------------------------------Results
	long long total_rays = _primary_rays + _reflection_rays + _shadow_rays;
	ostringstream render_time, throughput;
	render_time << fixed << setprecision(2) << render_seconds << " s";
	throughput << fixed << setprecision(1) << (total_rays / render_seconds) / 1e6 << " M rays/s";
	PrintField("Render time", render_time.str());
	PrintField("Rays traced", FormatCount(total_rays));
	PrintField("  primary", FormatCount(_primary_rays));
	PrintField("  reflection", FormatCount(_reflection_rays));
	PrintField("  shadow", FormatCount(_shadow_rays));
	PrintField("Throughput", throughput.str());

	if (_myimage.WriteToFile(_directory))
		PrintField("Saved to", _directory);
	else
		PrintField("Error", string("could not write ") + _directory);
	cout << endl;
}
