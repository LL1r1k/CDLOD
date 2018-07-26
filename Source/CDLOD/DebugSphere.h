#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "GameFramework/Actor.h"
#include "DebugSphere.generated.h"

UCLASS()
class CDLOD_API ADebugSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	ADebugSphere();

	UPROPERTY(EditAnywhere)
		int density = 2;

	UPROPERTY(EditAnywhere)
		float size = 100;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void CreateSphere();
	URuntimeMeshComponent* mesh;
};
