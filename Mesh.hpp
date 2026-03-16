#pragma once

#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>

// Represents a 3D mesh in the scene. To be fed to Image and rasturized.
class Mesh
{
public:
    std::vector<float3> vertices;                           /// List of 3D vert coordinates
    std::vector<std::array<int, 3>> faces;                  /// List of faces
    std::vector<float3> normals;                            /// List of normals as 3D Coords
    std::vector<std::array<int, 3>> faceNormals;            // List of faces for the normals
    float3 position { 0, 0, 0 };                            // Position of the mesh
    float3 rotation { 0, 0, 0 };                            // Euler angles in radians, applied X->Y->Z
    // Set base color of the mesh based on an input Color
    void SetColor(const Color& newColor) { color = newColor; }
    // Set base color of the mesh based on an R, G, B input
    void SetColor(uint8_t r, uint8_t g, uint8_t b) { color = Color(r,g,b); }
    // Load an OBJ file into the mesh
    void LoadOBJ(const std::string& path)
    {
        std::ifstream file(path);

        if (!file.is_open())
        {
            std::cerr << "ERROR: Could not open " << path << "\n";
            return;
        }
        std::string line;
        // Read obj file line by line
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string token;
            ss >> token;
            // Vertex coord
            if (token == "v")
            {
                float x, y, z;
                ss >> x >> y >> z;
                vertices.push_back({ x, y, z });
            }
            // Normal
            else if (token == "vn")
            {
                float x, y, z;
                ss >> x >> y >> z;
                normals.push_back({ x, y, z });
            }
            // Face
            else if (token == "f")
            {
                std::vector<int> posIndices;
                std::vector<int> normIndices;
                std::string vertToken;

                while (ss >> vertToken)
                {
                    // OBJ faces are like: 1/2/3, so split into 1 2 3
                    std::replace(vertToken.begin(), vertToken.end(), '/', ' ');
                    std::istringstream vss(vertToken);
                    // Read in each vert
                    int pos = 0, uv = 0, norm = 0;
                    vss >> pos;
                    vss >> uv;
                    vss >> norm;
                    // Convert to 0 base and store
                    posIndices.push_back(pos - 1);
                    normIndices.push_back(norm - 1);
                }
                // Fan triangulation
                for (int i = 1; i + 1 < (int)posIndices.size(); i++)
                {
                    faces.push_back({ posIndices[0], posIndices[i], posIndices[i+1] });
                    faceNormals.push_back({ normIndices[0], normIndices[i], normIndices[i+1] });
                }
            }
        }

        std::cout << "Loaded " << vertices.size() << " vertices, "
              << faces.size() << " faces from " << path << "\n";
        loaded = true;
    }
    // Send data loaded from the OBJ file to the Image
    void LoadTrianglesIntoImage(Image& image)
    {
        if (!loaded) return;
        std::cout << "Sending mesh triangles to image..." << std::endl;
        // Make sure OBJ has normals
        bool hasNormals = !normals.empty() && !faceNormals.empty();
        // Create a Triangle object for each face
        for (int faceIdx = 0; faceIdx < (int)faces.size(); faceIdx++)
        {
            const auto& face = faces[faceIdx];
            // Vertex
            float3 tv1 = TransformVertex(vertices[face[0]]);
            float3 tv2 = TransformVertex(vertices[face[1]]);
            float3 tv3 = TransformVertex(vertices[face[2]]);
            // Convert vertex to screen coords
            float2 p1 = ToScreenCoords(Project(tv1), image.width, image.height);
            float2 p2 = ToScreenCoords(Project(tv2), image.width, image.height);
            float2 p3 = ToScreenCoords(Project(tv3), image.width, image.height);
            // Normals
            float3 n1 = hasNormals ? TransformNormal(normals[faceNormals[faceIdx][0]]) : float3(0,0,1);
            float3 n2 = hasNormals ? TransformNormal(normals[faceNormals[faceIdx][1]]) : float3(0,0,1);
            float3 n3 = hasNormals ? TransformNormal(normals[faceNormals[faceIdx][2]]) : float3(0,0,1);
            // Create triangle
            Triangle tri(p1, p2, p3, tv1.z, tv2.z, tv3.z, n1, n2, n3, tv1, tv2, tv3);
            tri.SetColor(color);
            // Add created triangle to image
            image.AddTriangle(tri);
        }
        std::cout << "Mesh triangles sent!" << std::endl;
    }

private:
    // If OBJ has been loaded
    bool loaded = false;
    // Base color
    Color color{};
    // Transform vertex using rotation and translation
    float3 TransformVertex(const float3& v) const
    {
        float3 result = v;
        result = RotateX(result, rotation.x);
        result = RotateY(result, rotation.y);
        result = RotateZ(result, rotation.z);
        result = result + position;  // translate last
        return result;
    }
    // Rotate normals, but dont translate
    float3 TransformNormal(const float3& n) const
    {
        float3 result = n;
        result = RotateX(result, rotation.x);
        result = RotateY(result, rotation.y);
        result = RotateZ(result, rotation.z);
        return result;
    }
};