#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PlaneGenerator.generated.h"

UCLASS()
class CDLOD_API APlaneGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	APlaneGenerator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;	

	void CreateSection(FVector pos);
	void ConnectTriangles();
	void CalculateVertices();
	void GenerateMaterial();
	void ClearMesh();

	void UpdateComp();

	

private:

	URuntimeMeshComponent * Mesh;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> Colors;
	TArray<FRuntimeMeshTangent> Tangents;

};
