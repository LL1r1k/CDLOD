#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "PerlinNoise.h"

class CDLOD_API CubeFace
{
public:
	
	CubeFace(CubeFace* parent, FVector pos, FVector left, FVector forward, float size, int depth);
	CubeFace(FVector pos, FVector left, FVector forward, float size, int depth);
	CubeFace();
	~CubeFace();

	void Initialize(AActor* own, int density, float size, UMaterialInterface* mat, bool shaped, float noiseOffset, float noiseScale, int noiseSeed);
	void Generate();
	void CalculateVerts(TArray<FVector>* verts, TArray<FVector>* Normals, int steps);
	void RepairLOD(TArray<FVector>* verts, int steps);
	void CalculateTris(TArray<int32>* tris, int steps);
	FVector SpherifiedVector(FVector inVector);
	FVector SpherifiedCube(FVector inVector);
	UMaterialInstanceDynamic* CreateMaterial();

	void UpdateVisible();

	void ClearMesh();
	void Dispose();
	void Merge();

	void Hide();
	void Show();
	TArray<CubeFace*> SubDivide();

public:
	CubeFace* parent = nullptr;
	TArray<CubeFace*> childs;

	URuntimeMeshComponent* mesh = nullptr;
	PerlinNoise* perlinNoise = nullptr;

	float size;
	int depth;

	bool isGenerated = false;
	int32 needVisible = 0; // 0 - visible, 1 - need hide, 2 - hide
	bool haveChild = false;
	bool haveMesh = false;
	bool isChecked = false;
	bool inList = false;

	FVector A, B, C, D;

	FVector center;
	FVector position, left, forward;
};
