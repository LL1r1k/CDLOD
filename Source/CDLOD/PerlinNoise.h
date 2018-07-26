#pragma once

#include "CoreMinimal.h"

class CDLOD_API PerlinNoise
{
public:
	PerlinNoise();
	~PerlinNoise();

	void setNoiseSeed(const int32& newSeed);

	float SimplexNoise2D(float x, float y);
	float SimplexNoise3D(float x, float y, float z);
private:

	static unsigned char perm[];
	static float grad(int hash, float x, float y);
	static float grad(int hash, float x, float y, float z);
};
