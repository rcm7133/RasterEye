#pragma once

#include <vector>
#include <cstdint>
#include <fstream>
#include <string>
#include <limits>
#include <memory>
#include <chrono>
#include <thread>

#include "Triangle.hpp"
#include "utils/Color.hpp"
#include "utils/Math.hpp"
#include "shaders/Shader.hpp"

// Represents an image to be produced by the rasturizer.
class Image
{
public:
    int width;                                                /// Width of the image
    int height;                                               /// Height of the image
    Color background;                                         /// Color of the background
    std::vector<Color> pixels;                                /// Pixel array represented by Color
    std::vector<Triangle> triangles;                          /// Array of 2D triangles from meshes
    std::vector<float> depth;                                 /// Depth buffer
    std::unique_ptr<Shader> shader;                           /// Pointer to image shader
    // Delete default constructor
    Image() = delete;
    // Constructor with height and witdth
    Image(int w, int h) : width(w), height(h), pixels(w * h)
    {
        // Resize depth buffer based on height and width
        depth.resize(width * height, std::numeric_limits<float>::infinity());
    }
    // Set the pixel Color at the x,y coordinate given a Color input
    void SetPixel(int x, int y, const Color& color)
    {
        if (x < 0 || x >= width || y < 0 || y >= height) return;

        pixels[y * width + x] = color;
    }
    // Set the pixel Color at the x,y coordinate given an R, G, and B input
    void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) { SetPixel(x, y, Color(r,g,b)); }
    // Add a triangle to the list of triangles for the image
    void AddTriangle(const Triangle& newTriangle) { triangles.push_back(newTriangle); }
    // Set the background color for the image based on an input Color
    void SetBackgroundColor(const Color& color) { background = color; }
    // Set the background color for the image based on an R, G, and B input
    void SetBackgroundColor(uint8_t r, uint8_t g, uint8_t b) { SetBackgroundColor(Color(r,g,b)); }
    // Set the shader for the rasturizer
    void SetShader(std::unique_ptr<Shader> newShader) { shader = std::move(newShader); }
    // Clear the image data
    void Clear()
    {
        triangles.clear();
        std::fill(pixels.begin(), pixels.end(), background);
        std::fill(depth.begin(), depth.end(), std::numeric_limits<float>::infinity());
    }
    // Bake the image to a .ppm file
    void Bake(const std::string& file)
    {
        std::cout << "Baking Image..." << std::endl;
        // Start Timer
        auto start = std::chrono::high_resolution_clock::now();
        // Set up threads for multithreading
        std::vector<Color> output(width * height, background);
        std::vector<float> depthBuffer(width * height, std::numeric_limits<float>::infinity());
        unsigned int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(numThreads);
        int rowsPerThread = height / numThreads;

		// Render work lambda function
        auto renderChunk = [&](int startY, int endY)
        {
            // Loop through every triangle
            for (const Triangle& currTri : triangles)
            {
                // Create bounding box for tri
                int minX, minY, maxX, maxY;
                currTri.GetBounds(minX, minY, maxX, maxY, width, height);
                minY = std::max(minY, startY);
                maxY = std::min(maxY, endY - 1);
                // Loop through each pixel in the bounding box
                for (int y = minY; y <= maxY; y++)
                {
                    for (int x = minX; x <= maxX; x++)
                    {
                        // Check if the point is inside the triangle
                        float2 point((float)x, (float)y);
                        if (currTri.IsPointInside(point))
                        {
                            float z = currTri.InterpolateZ(point);
                            int flippedY = height - 1 - y;
                            int idx = flippedY * width + x;

                            if (z < depthBuffer[y * width + x])
                            {
                                depthBuffer[y * width + x] = z;
                                // Set the color of the pixel to what the shader determines, if any
                                Color finalColor = shader
                                    ? shader->Shade({ float2((float)x, (float)y), z,
                                      currTri.GetColor(),
                                      currTri.InterpolateNormal(point),
                                      currTri.InterpolateWorldPos(point) })
                                    : currTri.GetColor();

                                int flippedY = height - 1 - y;
                                output[flippedY * width + x] = finalColor;
                            }
                        }
                    }
                }
            }
        };

        // Start Threads
        for(unsigned int t = 0; t < numThreads; t++)
        {
            int startY = t * rowsPerThread;
            int endY = (t == numThreads - 1) ? height : startY + rowsPerThread;
            threads[t] = std::thread(renderChunk, startY, endY);
        }
        // Join threads
        for(auto& t : threads)
            t.join();

        // Write buffer to file after all threads finish
        std::string buf;
        buf.reserve(width * height * 12);

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                const Color& c = output[y * width + x];
                buf += std::to_string(c.r) + ' ' + std::to_string(c.g) + ' ' + std::to_string(c.b) + ' ';
            }
            buf += '\n';
        }

        std::ofstream out(file);
        out << "P3\n" << width << " " << height << "\n255\n";
        out.write(buf.data(), buf.size());

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = end - start;
        std::cout << "Bake completed in " << elapsed.count() << "s\n";
    }
};