#include "Vector3D.h"
#include "Sphere.h"
#include "Viewplane.h"
#include "Plane.h"
#include "Scene.h"
#include "EasyBMP.h"


using namespace std;

int main(int argc, char* argv[]){

	//-----------------------------------------------------------Defining variables
	// Scene units are roughly 10 cm: the room is 8 m wide, 3 m high and 10 m deep, with the floor at y = 0

	//Define pixels of image
	int pixel_x = 640, pixel_y = 480;

	// Camera at eye height near the front of the room, looking down at the spheres
	Vector3D camera(0, 16, -8);
	Vector3D look_at(0, 3, 38);
	Viewplane viewplane(pixel_x, pixel_y, 55, camera, look_at);

	// Lights hang just below the ceiling: a key light front-left, a fill front-right and a back light
	Vector3D lightsource1(-18, 28, 12);
	Vector3D lightsource2(22, 26, 2);
	Vector3D lightsource3(4, 28, 55);

	Scene scene(camera, viewplane, lightsource1);
	scene.AddLightsource(lightsource2);
	scene.AddLightsource(lightsource3);
	scene.SetLightIntensity(2.0);
	scene.SetMaxDepth(5);

	// Spheres resting on the floor: centre y = radius
	//        radius, centre,                  k_a,  k_d, k_s, alpha, R,   G,   B,   reflectivity
	Sphere mySphere1(5,   Vector3D(-9, 5, 42),   0.15, 0.6, 1.0, 120, 225, 225, 230, 0.85);  // chrome
	Sphere mySphere2(4,   Vector3D(5, 4, 34),    0.15, 0.9, 0.6, 60,  200, 30, 30, 0.12);    // red
	Sphere mySphere3(3.5, Vector3D(15, 3.5, 47), 0.15, 0.9, 0.8, 80,  230, 175, 40, 0.45);   // gold
	Sphere mySphere4(3,   Vector3D(-2, 3, 27),   0.15, 0.9, 0.6, 60,  35, 80, 210, 0.12);    // blue
	Sphere mySphere5(2,   Vector3D(-12, 2, 29),  0.15, 0.9, 0.5, 50,  40, 170, 70, 0.08);    // green
	Sphere mySphere6(1.5, Vector3D(8, 1.5, 25),  0.15, 0.9, 0.5, 50,  150, 50, 200, 0.08);   // purple
	Sphere mySphere7(6,   Vector3D(-24, 6, 62),  0.15, 0.9, 0.4, 40,  30, 170, 175, 0.1);    // teal

	scene.AddSphere(mySphere1);
	scene.AddSphere(mySphere2);
	scene.AddSphere(mySphere3);
	scene.AddSphere(mySphere4);
	scene.AddSphere(mySphere5);
	scene.AddSphere(mySphere6);
	scene.AddSphere(mySphere7);

	// A closed room, so reflections always land on a surface rather than empty black space
	Vector3D floor_normal(0, 1, 0), floor_point(0, 0, 0);
	Vector3D ceiling_normal(0, -1, 0), ceiling_point(0, 30, 0);
	Vector3D left_normal(1, 0, 0), left_point(-40, 0, 0);
	Vector3D right_normal(-1, 0, 0), right_point(40, 0, 0);
	Vector3D back_normal(0, 0, -1), back_point(0, 0, 80);
	Vector3D front_normal(0, 0, 1), front_point(0, 0, -20);

	//     normal, point,                     k_a,  k_d, k_s,  alpha, R,   G,   B,   reflectivity
	Plane floor(floor_normal, floor_point,     0.15, 0.8, 0.3,  40,  205, 205, 200, 0.2);
	Plane ceiling(ceiling_normal, ceiling_point, 0.3, 0.6, 0.0, 1,  235, 235, 230);
	Plane left_wall(left_normal, left_point,   0.2, 0.7, 0.05, 10,  200, 190, 175);
	Plane right_wall(right_normal, right_point, 0.2, 0.7, 0.05, 10, 200, 190, 175);
	Plane back_wall(back_normal, back_point,   0.2, 0.45, 0.05, 10, 165, 172, 185);
	Plane front_wall(front_normal, front_point, 0.2, 0.7, 0.05, 10, 200, 190, 175);

	// 50 cm floor tiles give the eye a sense of scale and perspective
	floor.SetChecker(95, 95, 100, 5);

	scene.AddPlane(floor);
	scene.AddPlane(ceiling);
	scene.AddPlane(left_wall);
	scene.AddPlane(right_wall);
	scene.AddPlane(back_wall);
	scene.AddPlane(front_wall);

	// Output path can be given as the first argument
	scene.SetDirectory(argc > 1 ? argv[1] : "Render.bmp");
	scene.Engage();

}
