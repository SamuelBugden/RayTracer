#include "Vector3D.h"
#include "Sphere.h"
#include "Viewplane.h"
#include "Plane.h"
#include "Scene.h"
#include "EasyBMP.h"


using namespace std;

int main(int argc, char* argv[]){

	//-----------------------------------------------------------Defining variables

	//Define pixels of image
	int pixel_x = 500, pixel_y = 500;

	Vector3D camera(0, -5, 1);
	Vector3D viewplane_pos(0, -5, 5);
	Vector3D lightsource1(-20, 5, 5);
	Vector3D lightsource2(15,1,0);
	Vector3D lightsource3(-3, 40, 40);

	Viewplane viewplane(pixel_x, pixel_y, 5, 5, viewplane_pos);

	Scene scene(camera, viewplane, lightsource1);

	// Spheres resting on the floor (y = -10): centre y = -10 + radius
	//        radius, centre,                    k_a, k_d, k_s, alpha, R,   G,   B,   reflectivity
	Sphere mySphere1(5,   Vector3D(-8, -5, 35),    0.2, 1, 0.9, 20, 220, 40, 40, 0.2);    // red
	Sphere mySphere2(4,   Vector3D(6, -6, 30),     0.2, 1, 0.9, 30, 230, 180, 30, 0.5);   // gold
	Sphere mySphere3(3,   Vector3D(0, -7, 22),     0.2, 1, 0.9, 15, 40, 90, 230, 0.2);    // blue
	Sphere mySphere4(2,   Vector3D(-4, -8, 17),    0.2, 1, 0.9, 14, 40, 200, 70, 0.1);    // green
	Sphere mySphere5(1.5, Vector3D(3.5, -8.5, 15), 0.2, 1, 0.9, 14, 170, 60, 220, 0.1);   // purple
	Sphere mySphere6(8,   Vector3D(-17, -2, 58),   0.2, 1, 0.9, 25, 30, 200, 200, 0.2);   // cyan
	Sphere mySphere7(6,   Vector3D(16, -4, 52),    0.2, 1, 0.9, 25, 240, 240, 240, 0.8);  // white

	Vector3D plane_normal1(0, 1, 0), point_on_plane1(0, -10, 0);
	Vector3D plane_normal2(0, -1, 0), point_on_plane2(0, 30, 0);
	Vector3D plane_normal3(1, 0, 0), point_on_plane3(-30, 0, 0);
	Vector3D plane_normal4(-1, 0, 0), point_on_plane4(30, 0, 0);
	Vector3D plane_normal5(0, 0, -1), point_on_plane5(0, 0, 70);

	Plane plane1(plane_normal1, point_on_plane1, 0.2, 0.8, 0.4, 5, 150, 150, 150, 0.3);
	Plane plane2(plane_normal2, point_on_plane2, 0.2, 0.2, 0.2, 5, 100, 150, 250);
	Plane plane3(plane_normal3, point_on_plane3, 0.2, 0.8, 0.5, 5, 100, 150, 250);
	Plane plane4(plane_normal4, point_on_plane4, 0.2, 0.2, 0.5, 5, 100, 150, 250);
	Plane plane5(plane_normal5, point_on_plane5, 0.2, 0.2, 0.1, 5, 100, 150, 250);

	scene.AddSphere(mySphere1);
	scene.AddSphere(mySphere2);
	scene.AddSphere(mySphere3);
	scene.AddSphere(mySphere4);
	scene.AddSphere(mySphere5);
	scene.AddSphere(mySphere6);
	scene.AddSphere(mySphere7);

	scene.AddPlane(plane1);
	scene.AddPlane(plane2);
	scene.AddPlane(plane3);
	scene.AddPlane(plane4);
	scene.AddPlane(plane5);

	scene.AddLightsource(lightsource2);
	scene.AddLightsource(lightsource3);

	// Output path can be given as the first argument
	scene.SetDirectory(argc > 1 ? argv[1] : "Render.bmp");
	scene.Engage();
	//-----------------------------------------------------------Tests

}