#include "Scene.h"
#include <iostream>
#include <string>
using namespace std;

Scene::Scene(Vector3D camera, Viewplane viewplane, Vector3D lightsource)
{
	_camera = camera;
	_lightsourcelist.push_back(lightsource);
	_viewplane = viewplane;
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
void Scene::SetDirectory(char* directory)
{
	_directory = directory;
}
int Scene::CalcNearestSurface(vector<Ray> raylist)
{
	int index = 0;
	double smallest = (raylist[0].GetPoint() - _camera).Magnitude();
	for (int x = 1; x < (int)raylist.size(); x++){
		Ray reflected = raylist[x];
		double distance_to_surface= (reflected.GetPoint() - _camera).Magnitude();
		if (distance_to_surface < smallest){
			smallest = distance_to_surface;
			index = x;
		}
	}
	return index;
}
double* Scene::normalisedIllumination(Sphere sphere, double ambient, double diffusive, double specular)
{
	int Red_colour = sphere.GetRed();
	int Green_colour = sphere.GetGreen();
	int Blue_colour = sphere.GetBlue();

	double red_component = (Red_colour * (ambient + diffusive)) + (255 * specular);
	double green_component = (Green_colour * (ambient + diffusive)) + (255 * specular);
	double blue_component = (Blue_colour * (ambient + diffusive)) + (255 * specular);

	double illumination_total = red_component + green_component + blue_component;
	double overload = illumination_total - (255 * 3);

	if (overload > 0){
		red_component = red_component + ((overload * Red_colour) / 2);
		green_component = green_component + ((overload * Green_colour) / 2);
		blue_component = blue_component + ((overload * Blue_colour) / 2);
	}
	double illumination[3] = { red_component, green_component, blue_component };
	return illumination;
}
double* Scene::normalisedIllumination(Plane plane, double ambient, double diffusive, double specular)
{
	int Red_colour = plane.GetRed();
	int Green_colour = plane.GetGreen();
	int Blue_colour = plane.GetBlue();

	double red_component = (Red_colour * (ambient + diffusive)) + (255 * specular);
	double green_component = (Green_colour * (ambient + diffusive)) + (255 * specular);
	double blue_component = (Blue_colour * (ambient + diffusive)) + (255 * specular);

	double illumination_total = red_component + green_component + blue_component;
	double overload = illumination_total - (255 * 3);

	if (overload > 0){
		red_component = red_component + ((overload * Red_colour) / 2);
		green_component = green_component + ((overload * Green_colour) / 2);
		blue_component = blue_component + ((overload * Blue_colour) / 2);
	}
	double illumination[3] = { red_component, green_component, blue_component };
	return illumination;
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
			for (int k = 0; k < (int)_spherelist.size(); k++){
				bool intersect_sphere = _spherelist[k].intersect(_viewplane.GetPixelRay(_camera, i, j));
				if (intersect_sphere == true){
					Ray reflected = _spherelist[k].Reflect(_viewplane.GetPixelRay(_camera, i, j));
					_raylist.push_back(reflected);
					_indexlist.push_back(k);
					_objecttype.push_back(1);
				}
			}
			for (int p = 0; p < (int)_planelist.size(); p++){
				bool intersect_plane = _planelist[p].intersect(_viewplane.GetPixelRay(_camera, i, j));
				if (intersect_plane == true){
					Ray reflected = _planelist[p].reflected(_viewplane.GetPixelRay(_camera, i, j));
					_raylist.push_back(reflected);
					_indexlist.push_back(p);
					_objecttype.push_back(0);
				}
			}
			double diffusive = 0, specular = 0;
			double illumination[3];
			int size = _raylist.size();
			if (size == 0){
				illumination[0] = 0;
				illumination[1] = 0;
				illumination[2] = 0;
			}
			else{
				int index_r, index_s, objtype;
				if (size == 1){
					index_r = 0;
					index_s = _indexlist[0];
					objtype = _objecttype[0];
				}
				else{
					index_r = CalcNearestSurface(_raylist);
					index_s = _indexlist[index_r];
					objtype = _objecttype[index_r];
				}
				Vector3D normal_at_point;
				if (objtype == 1)
					normal_at_point = (_raylist[index_r].GetPoint() - _spherelist[index_s].GetCentre()).MakeUnitVector();
				else if (objtype == 0)
					normal_at_point = (_planelist[index_s].GetNormal());
				for (int l = 0; l < (int)_lightsourcelist.size(); l++)
				{
					Vector3D unit_to_source = (_lightsourcelist[l] - _raylist[index_r].GetPoint()).MakeUnitVector();
					Ray ray_to_light(unit_to_source, _raylist[index_r].GetPoint());
					bool shadowcheck = false;
					int index_temp;
					if (objtype == 1)
						index_temp = index_s;
					if (objtype == 0)
						index_temp = -1;
						
					for (int m = 0; m < (int)_spherelist.size(); m++){
						if (m != index_temp){ // index_sphere
							if (_spherelist[m].checkShadow(ray_to_light,_lightsourcelist[l]) == true)
								shadowcheck = true;
						}
					}
					if (shadowcheck == false){
						if (objtype == 1){
							diffusive += (_spherelist[index_s].GetK_d() * max(0.0, (unit_to_source.DotProduct(normal_at_point))));
							specular += (_spherelist[index_s].GetK_s() * pow(max(0.0, unit_to_source.DotProduct(_raylist[index_r].GetUnit())), _spherelist[index_s].GetAlpha()));
						}
						else if (objtype == 0){
							
							diffusive += (_planelist[index_s].GetK_d() * max(0.0, (unit_to_source.DotProduct(normal_at_point))));
							specular += (_planelist[index_s].GetK_s() * pow(max(0.0, unit_to_source.DotProduct(_raylist[index_r].GetUnit())), _planelist[index_s].GetAlpha()));
						}
					}
				}
				if (objtype == 1){
					double ambient = (_spherelist[index_s].GetK_a());
					double *illumination_array = normalisedIllumination(_spherelist[index_s], ambient, diffusive, specular);
					illumination[0] = illumination_array[0];
					illumination[1] = illumination_array[1];
					illumination[2] = illumination_array[2];
				}
				else if (objtype == 0){
					double ambient = (_planelist[index_s].GetK_a());
					double *illumination_array = normalisedIllumination(_planelist[index_s], ambient, diffusive, specular);
					illumination[0] = illumination_array[0];
					illumination[1] = illumination_array[1];
					illumination[2] = illumination_array[2];
				}
			}
			
			_raylist.clear();
			_indexlist.clear();
			_objecttype.clear();
			inputBMP(illumination, i,j);
		}
	}
	cout << "\nFinalising.." << endl;
	_myimage.WriteToFile(_directory);
	cout << "Render complete! \nImage Location ->  "<< _directory << endl;
}