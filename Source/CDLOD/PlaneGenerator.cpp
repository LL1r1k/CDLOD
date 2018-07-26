#include "PlaneGenerator.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

APlaneGenerator::APlaneGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlaneGenerator::BeginPlay()
{
	Super::BeginPlay();	
}

void APlaneGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlaneGenerator::CreateSection(FVector pos)
{	
	Mesh = NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass());
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Mesh->RegisterComponent();

	Vertices.AddUninitialized(4);
	Triangles.AddUninitialized(6);

	Vertices[0] = FVector(-100, 100, 0);
	Vertices[0] += pos;
	Vertices[1] = FVector(100, 100, 0);
	Vertices[1] += pos;
	Vertices[2] = FVector(100, -100, 0);
	Vertices[2] += pos;
	Vertices[3] = FVector(-100, -100, 0);
	Vertices[3] += pos;

	Triangles[0] = 0;
	Triangles[4] = 2;
	Triangles[1] = Triangles[3] = 1;
	Triangles[2] = Triangles[5] = 3;

	Mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, Colors, Tangents, true);
}

void APlaneGenerator::ConnectTriangles()
{

}

void APlaneGenerator::CalculateVertices()
{

}

void APlaneGenerator::GenerateMaterial()
{

}

void APlaneGenerator::ClearMesh()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UVs.Empty();
	Colors.Empty();
	Tangents.Empty();
	//Mesh->ClearMeshSection(0);
}

void APlaneGenerator::UpdateComp()
{
	print("Update");
}

