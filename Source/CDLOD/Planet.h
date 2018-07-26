#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "PerlinNoise.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

UCLASS()
class CDLOD_API APlanet : public AActor
{
	GENERATED_BODY()
public:	
	void InitializeStatic(int dens, float size, UMaterialInterface* material, bool shaped, TArray<FColor> color);
	void InitializeNoise(float noiseOff, float noiseScl, int32 noiseseed, int octav, float lucanar, float gai);
	void Initialize(FVector pos, FVector left, FVector forward, float size, int depth, APlanet* parent);
	void Destuctor();

	void ShowNieghbours();
	bool UpdateNieghbours();

	void Generate();
	void UpdateMesh();
	void UpdateVisible();
	void RepaitLOD(TArray<APlanet*>* activeTree);
	void Show();
	void Hide();

	void DeleteActor();
	void Dispose();
	void Merge(TArray<APlanet*>* activeTree);
	TArray<APlanet*> SubDivide();
	APlanet();

public:
	APlanet* parent = nullptr;
	TArray<APlanet*> childs;
	UPROPERTY(VisibleAnywhere)
		TArray<APlanet*> Nieghbours;
	UPROPERTY(VisibleAnywhere)
		TArray<bool> needBorder;
	
	float size;
	UPROPERTY(VisibleAnywhere)
		int depth;

	bool necessUpdate = false;
	bool isUpdated = false;
	UPROPERTY(VisibleAnywhere)
		bool isGenerated = false;
	bool needUpdate = true;
	bool cheackniegh = false;
	int32 needVisible = 0; // 0 - visible, 1 - need hide, 2 - hide
	bool haveChild = false;
	bool haveMesh = false;
	bool isChecked = false;
	bool inList = false;	

	FVector A, B, C, D;

	FVector position, left, forward;

private:
	bool LeftBorderCreated = false;
	bool RightBorderCreated = false;
	bool DownBorderCreated = false;
	bool UpBorderCreated = false;

	void AddVertex(FVector position, FVector2D UV, int vetrexIndex);
	void AddTriangle(int a, int b, int c);
	void CalculateVerts();
	void CalculateTris();
	void CalculateNormals();

	FVector SurfaceNormalFromIndices(int indexA, int indexB, int indexC);
	UMaterialInstanceDynamic* CreateMaterial();
	FVector SpherifiedVector(FVector inVector);
	FVector Spherified(FVector inVector);
	float noise(FVector point);		

	int verticesPerLine;
	TArray<FVector> Normals;
	TArray<int>	vertexIndexMap;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> BorderVertices;
	TArray<int32> BorderTriangles;
	TArray<FVector2D> UVs;
	TArray<FColor> VertexColor;
	TArray<FRuntimeMeshTangent> Tangents;

	TArray<FVector> UpBorder;
	TArray<FVector> DownBorder;
	TArray<FVector> LeftBorder;
	TArray<FVector> RightBorder;

	PerlinNoise* plane = nullptr;
	UMaterialInstanceDynamic * baseMaterial = nullptr;
	URuntimeMeshComponent* mesh = nullptr;
	USphereComponent* transform;
};
