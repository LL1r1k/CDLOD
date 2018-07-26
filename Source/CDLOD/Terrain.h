#pragma once

#include "CoreMinimal.h"
#include "CubeFace.h"
#include "Planet.h"
#include "PlaneGenerator.h"
#include "RuntimeMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

UCLASS()
class CDLOD_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	ATerrain();

	UPROPERTY(EditAnywhere, Category = "Generate")
		bool GenerateSphere = true;

	UPROPERTY(EditAnywhere, Category = "Generate")
		bool DeleteSphere = false;

	UPROPERTY(EditAnywhere, Category = "Generate")
		bool ChangeSeed = false;

	UPROPERTY(EditAnywhere, Category = "Draw")
		UMaterialInterface* material;

	UPROPERTY(EditAnywhere, Category = "Draw")
		TArray<FColor> BaseColor;

	UPROPERTY(EditAnywhere, Category = "Draw")
		bool generate = true;

	UPROPERTY(EditAnywhere, Category = "Draw")
		float CubeSize = 100;

	UPROPERTY(EditAnywhere, Category = "Draw")
		int Density = 1;	

	UPROPERTY(EditAnywhere, Category = "Noise")
		float NoiseScale = 1;

	UPROPERTY(EditAnywhere, Category = "Noise")
		float NoiseOffset = 1;

	UPROPERTY(EditAnywhere, Category = "Noise")
		int NoiseOctaves = 8;

	UPROPERTY(EditAnywhere, Category = "Noise")
		float NoiseLucanarity = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Noise")
		float NoiseGain = 0.5f;	

	UPROPERTY(EditAnywhere, Category = "Noise")
		int32 NoiseSeed = 0;

	UPROPERTY(EditAnywhere, Category = "Draw")
		float constant = 1.2;

	UPROPERTY(EditAnywhere, Category = "Draw")
		int MaxDepth = 5;

	UPROPERTY(EditAnywhere, Category = "Draw")
		bool Shaped = true;

	UPROPERTY(EditAnywhere, Category = "Draw")
		bool CameraCulling = true;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	

	void ResetActors();

	virtual void Tick(float DeltaTime) override;
	void GetActiveQuadTreeFaces(APlanet* face, TArray<APlanet*>* activeTree);
	float ClosestDistance(FVector point,APlanet* poly, FVector *Closestpoint);
	void SpherifiedCube(FVector* inVector);
	void Update(int face, bool first);	
	bool InCameraFrustum(FVector WorldLocation);
	bool InCameraFrustum(FVector A, FVector B, FVector C, FVector D, int tolerances);
	
	bool ProjectWorldToScren(UPlayer* Player, FVector WorldLocation, FVector2D& ScreenLocation);
	bool IsObjectOnTop(FVector start, FVector object);

	FVector pos;
	FVector left;
	FVector forward;

	int32 ScreenWidth = 0;
	int32 ScreenHeight = 0;

	FVector oldposition;
	FVector newposition;

	APlayerController* PlayerController;
	APawn* player;
	TArray<APlanet*> Planet;

	UPROPERTY(VisibleAnywhere)
		USphereComponent * SphereTransform;

};
