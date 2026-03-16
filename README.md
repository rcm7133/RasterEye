# RasterEye

C++ Multithreaded 3D Rasterizer supporting OBJ file based models. Uses custom shaders to output a ppm image file.

## How to use:
In the root directory run:
- Make: Compile the rasterizer
- ./main: Run the rasterizer
- Make clean: Delete the compiled out file

main.cpp can be edited to load any desired OBJ files. OBJ files should be put in the models folder.

## How does it work? 
RasterEye implements a basic form of rasterization. Rasterization is the process of converting a 3D scene into a 2D array of color to be displayed on screen. The Mesh class reads in an OBJ file line by line, creating Triangles that are fed into the scene. When the rasterizer is told to drawn an image it checks each triangle in the scene. The triangle's bounding box checked for each pixel to see if that point is inside of the triangle. If it is then the color of the triangle is determined. The color of the triangle is determined by the Shader class, and can be customized to shade a triangle as desired. By default Lambertian Diffuse is applied for basic shading. If the point is not inside of the triangle then the backgrund color of the scene is applied. 

This started out with just a basic triangle with 3 2D points. 

