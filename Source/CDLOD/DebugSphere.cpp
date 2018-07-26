#include "DebugSphere.h"


ADebugSphere::ADebugSphere()
{
	PrimaryActorTick.bCanEverTick = true;
	mesh = CreateDefaultSubobject<URuntimeMeshComponent>(FName("Mesh"));
	mesh->RegisterComponent();
}

void ADebugSphere::BeginPlay()
{
	Super::BeginPlay();

}

void ADebugSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CreateSphere();
}

void ADebugSphere::CreateSphere()
{
	FVector start = FVector(-1, -1, -1);
	FVector left = FVector(2, 0, 0);
	FVector forward = FVector(0, 2, 0);

	float step = 1 / (float)density;
	for (int y = 0, i = 0; y < density +1; y++)
	{
		for (int x = 0; x < density +1; x++, i++)
		{
			FVector vPosition = start;
			vPosition += (y * left + x * forward) * step;

			FVector p2 = vPosition * vPosition;
			FVector n
			(
			vPosition.X * sqrt(1.0f - 0.5f * (p2.Z + p2.Y) + p2.Z*p2.Y / 3.0f),
			vPosition.Y * sqrt(1.0f - 0.5f * (p2.Z + p2.X) + p2.Z*p2.X / 3.0f),
			vPosition.Z * sqrt(1.0f - 0.5f * (p2.X + p2.Y) + p2.X*p2.Y / 3.0f)
			);


			/*vPosition.Normalize();
			vPosition *= size;*/

			DrawDebugPoint(GetWorld(), n * size, 10, FColor::Purple);
		}
	}

}

