#pragma once

#include "../utils/Color.hpp"
#include "../utils/float2.hpp"
#include "../utils/Math.hpp"
#include "Light.hpp"

// Holds input data to be fed to a shader
struct ShaderInput
{
    float2 screnPos;
    float z;
    Color baseColor;
    float3 normal;
    float3 worldPos;
};

// Base class that determines the final color output for a pixel
class Shader
{
public:
    virtual ~Shader() = default;
    virtual Color Shade(const ShaderInput& input) const = 0;
};

// Colors an unshaded flat color
class FlatShader : public Shader
{
public:
    Color Shade(const ShaderInput& input) const override
    {
        return input.baseColor;
    }
};

// Inverts colors on the screen
class InvertColorShader : public Shader
{
public:
    Color Shade(const ShaderInput& input) const override
    {
        return Color(
            (uint8_t)(255 - input.baseColor.r),
            (uint8_t)(255 - input.baseColor.g),
            (uint8_t)(255 - input.baseColor.b)
        );
    }
};

// Colors in with Lambertian Diffuse
class DiffuseShader : public Shader
{
public:
    DiffuseShader(const Light& light) : light(light) {}

    Color Shade(const ShaderInput& input) const override
    {
        // Direction to light source
        float3 toLight = {
            light.position.x - input.worldPos.x,
            light.position.y - input.worldPos.y,
            light.position.z - input.worldPos.z
        };

        // Normalize
        float len = sqrtf(Dot3(toLight, toLight));
        if (len > 0.0f)
            toLight = { toLight.x/len, toLight.y/len, toLight.z/len };

        float diff = std::max(0.0f, Dot3(input.normal, toLight));

        // Tint by light color
        return Color(
            (uint8_t)(input.baseColor.r * diff * (light.color.r / 255.0f)),
            (uint8_t)(input.baseColor.g * diff * (light.color.g / 255.0f)),
            (uint8_t)(input.baseColor.b * diff * (light.color.b / 255.0f))
        );
    }

private:
    Light light;
};