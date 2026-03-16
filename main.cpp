#include <iostream>
#include "Image.hpp"
#include "Triangle.hpp"
#include "utils/Math.hpp"
#include "utils/float2.hpp"
#include "Mesh.hpp"
#include "shaders/Shader.hpp"
#include "shaders/Light.hpp"

int main()
{
	// Set up image
	Image img(512, 512);
	Color background = Color(255, 255, 255);
	img.SetBackgroundColor(background);

	// Load Meshes
	Mesh sphere;
	sphere.LoadOBJ("models/sphere-highpoly.obj");
	sphere.position = { -0.5, 0, 2 };
	sphere.rotation = { 2, 15, 2 };
	sphere.SetColor({ 5, 212, 125} );
	sphere.LoadTrianglesIntoImage(img);

	Mesh cube;
	cube.LoadOBJ("models/cube.obj");
	cube.position = { 1, 0, 3.5 };
	cube.rotation = { 5, 15, 2 };
	cube.SetColor({ 124, 55, 52} );
	cube.LoadTrianglesIntoImage(img);

	// Light
    Light sun { {-4, 1, -4}, Color(255, 255, 255) };
	// Load shaders
    img.SetShader(std::make_unique<DiffuseShader>(sun));
	// Bake image
    img.Bake("output/output.ppm");
    img.Clear();
	return 0;
}

