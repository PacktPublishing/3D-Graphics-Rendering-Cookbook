#pragma once
#include "shared/scene/vec4.h"

// Common definitions for Lights and Cameras

struct PACKED_STRUCT CameraProperties
{
	gpumat4 projection_ = gpumat4(glm::mat4(1.f));
	gpumat4 view_       = gpumat4(glm::mat4(1.f));
	gpumat4 model_      = gpumat4(glm::mat4(1.f));
	gpuvec4 position_   = gpuvec4(glm::vec4(0.0f));
};

static_assert(sizeof(CameraProperties) == (3 * 4 * sizeof(gpuvec4) + sizeof(gpuvec4)), "Invalid sizeof(CameraProperties), should be 3 * sizeof(mat4) + sizeof(gpuvec4)");

struct PACKED_STRUCT LightProperties
{
	gpuvec4 ambientColor_ = gpuvec4( 0.f );
	gpuvec4 color_        = gpuvec4( 0.f );
	// x = light radius, for point and spot lights; x = attenuation cutoff; 2 flag-like floats [scattering + cast shadow]
	gpuvec4 attenuationParams_       = gpuvec4(10.0f, 2.0f / 255.0f, 0.f, 0.f);
	gpuvec4  FDirection = gpuvec4( 0.f, 0.f, -1.f, 0.f );
	// inner angle, outer angle, falloff
	gpuvec4  FSpotRange = gpuvec4( 0.f, 90.f, 1.f, 0.f );
	// (mapSize, mapCount, splitLambdaParam, strength)
	gpuvec4 FShadowMapSize = gpuvec4(1024.f, 0.f, 0.5f, 1.f);
	// const, slope, 0, 0 [ or filterType + shadowUpdateInterval]
	gpuvec4 shadowBias_ = gpuvec4( -0.0001f, 0.f, 0.f, 0.f );
};

static_assert(sizeof(LightProperties) == 7 * sizeof(gpuvec4), "Invalid sizeof(LightProperties), should be 7 * sizeof(gpuvec4)");
