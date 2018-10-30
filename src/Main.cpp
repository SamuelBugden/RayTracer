#include "Vector3D.h"
#include "Sphere.h"
#include "Viewplane.h"
#include "Plane.h"
#include "Scene.h"
#include "EasyBMP.h"


using namespace std;

int main(){

	//-----------------------------------------------------------Defining variables

	//Define pixels of image
	int pixel_x = 100, pixel_y = 100;

	Vector3D camera(0, -5, 1);
	Vector3D viewplane_pos(0, -5, 5);
	Vector3D lightsource1(-20, 5, 5);
	Vector3D lightsource2(15,1,0);
	Vector3D lightsource3(-3, 40, 40);

	Viewplane viewplane(pixel_x, pixel_y, 5, 5, viewplane_pos);

	Scene scene(camera, viewplane, lightsource1);

	Vector3D sphere1(-9, 2, 51);
	Vector3D sphere2(-2, -4, 33);
	Vector3D sphere3(2, -7, 20);
	Vector3D sphere4(3.5, -9, 15);

	Sphere mySphere1(12, sphere1, 0.2, 1, 0.9, 20, 200, 200, 0);
	Sphere mySphere2(6, sphere2, 0.2, 1, 0.9, 15, 200, 0, 0);
	Sphere mySphere3(3, sphere3, 0.2, 1, 0.9, 14, 0, 200, 200);
	Sphere mySphere4(1, sphere4, 0.2, 1, 0.9, 14, 0, 200, 0);

	Vector3D plane_normal1(0, 1, 0), point_on_plane1(0, -10, 0);
	Vector3D plane_normal2(0, -1, 0), point_on_plane2(0, 30, 0);
	Vector3D plane_normal3(1, 0, 0), point_on_plane3(-30, 0, 0);
	Vector3D plane_normal4(-1, 0, 0), point_on_plane4(30, 0, 0);
	Vector3D plane_normal5(0, 0, -1), point_on_plane5(0, 0, 70);

	Plane plane1(plane_normal1, point_on_plane1, 0.2, 0.8, 0.4, 5, 150, 150, 150);
	Plane plane2(plane_normal2, point_on_plane2, 0.2, 0.2, 0.2, 5, 100, 150, 250);
	Plane plane3(plane_normal3, point_on_plane3, 0.2, 0.8, 0.5, 5, 100, 150, 250);
	Plane plane4(plane_normal4, point_on_plane4, 0.2, 0.2, 0.5, 5, 100, 150, 250);
	Plane plane5(plane_normal5, point_on_plane5, 0.2, 0.2, 0.1, 5, 100, 150, 250);

	

	scene.AddSphere(mySphere1);
	scene.AddSphere(mySphere2);
	scene.AddSphere(mySphere3);
	scene.AddSphere(mySphere4);

	scene.AddPlane(plane1);
	scene.AddPlane(plane2);
	scene.AddPlane(plane3);
	scene.AddPlane(plane4);
	scene.AddPlane(plane5);

	scene.AddLightsource(lightsource2);
	scene.AddLightsource(lightsource3);

	scene.SetDirectory("D:\Pictures\\Trial.bmp");
	scene.Engage();
	//-----------------------------------------------------------Tests

}