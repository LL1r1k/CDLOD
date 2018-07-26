#include "CubeFace.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

static AActor* owner;
static UMaterialInterface* material;
static int density = 1;
static float fullSize = 100;
static bool shape = false;
static float noiseOffset = 1;
static float noiseScale = 1;
static int32 noiseSeed = 0;

CubeFace::CubeFace(CubeFace* parent, FVector pos, FVector left, FVector forward, float size, int depth)
{
	this->parent = parent;
	position = pos;
	this->left = left;
	this->forward = forward;
	this->size = size;
	center = position + left * size / 2 + forward * size / 2;
	perlinNoise = new PerlinNoise();
	if (noiseSeed != 0)
	{
		perlinNoise->setNoiseSeed(noiseSeed);
	}
	if (shape)
	{
		center = SpherifiedVector(center);

		A = position;
		A = SpherifiedVector(A);

		B = position + size * left;
		B = SpherifiedVector(B);

		C = position + size * forward;
		C = SpherifiedVector(C);

		D = position + size * forward + size * left;
		D = SpherifiedVector(D);
	}
	else
	{
		A = position;
		B = position + size * left;
		C = position + size * forward;
		D = position + size * forward + size * left;
	}
	this->depth = depth;
}

CubeFace::CubeFace(FVector pos, FVector left, FVector forward, float size, int depth)
{
	position = pos;
	this->left = left;
	this->forward = forward;
	this->size = size;
	center = position + left * size / 2 + forward * size / 2;
	perlinNoise = new PerlinNoise();
	if (noiseSeed != 0)
	{
		perlinNoise->setNoiseSeed(noiseSeed);
	}
	if (shape)
	{
		center = SpherifiedVector(center);

		A = position;
		A = SpherifiedVector(A);

		B = position + size * left;
		B = SpherifiedVector(B);

		C = position + size * forward;
		C = SpherifiedVector(C);

		D = position + size * forward + size * left;
		D = SpherifiedVector(D);
	}
	else
	{
		A = position;
		B = position + size * left;
		C = position + size * forward;
		D = position + size * forward + size * left;
	}
	this->depth = depth;
}

CubeFace::CubeFace()
{
	
}

CubeFace::~CubeFace()
{
	if (childs.Num() > 0)
	{
		for (CubeFace* face : childs)
		{
			delete face;
		}
		childs.Empty();
	}
	if (mesh)
	{
		mesh->ClearMeshSection(0);
		mesh->DestroyComponent();
		mesh = nullptr;
	}	
	if (parent)
	{
		parent = nullptr;
	}
	if (perlinNoise)
	{
		delete perlinNoise;
		perlinNoise = nullptr;
	}
}

void CubeFace::Initialize(AActor* own,int dens, float size, UMaterialInterface* mat, bool shaped, float noiseOff, float noiseScl, int32 noiseseed)
{
	owner = own;
	density = dens;
	fullSize = size;
	material = mat;
	shape = shaped;
	noiseOffset = noiseOff;
	noiseScale = noiseScl;
	noiseSeed = noiseseed;
}

void CubeFace::Generate()
{
	if (isGenerated)
	{
		mesh->SetVisibility(true);
		return;
	}

	if (haveMesh)
	{
		mesh->SetVisibility(true);
		isGenerated = true;
		return;
	}
	
	if (!mesh)
	{
		mesh = NewObject<URuntimeMeshComponent>(owner);
		mesh->SetupAttachment(owner->GetRootComponent());
		mesh->RegisterComponent();
	}

	TArray<FVector> Vertices;
	TArray<int32>Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs ;
	TArray<FColor> Color;
	TArray<FRuntimeMeshTangent> Tangents;

	int steps = density + 1;
	Vertices.AddUninitialized(steps * steps);
	Normals.AddUninitialized(steps * steps);
	Triangles.AddUninitialized(density * density * 6);

	CalculateVerts(&Vertices, &Normals, steps);
	//RepairLOD(&Vertices, steps);
	CalculateTris(&Triangles, steps);

	mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, Color, Tangents, true, EUpdateFrequency::Infrequent);
	mesh->SetMaterial(0, CreateMaterial());

	isGenerated = true;
	haveMesh = true;
	Vertices.Empty();
	Normals.Empty();
	Triangles.Empty();
}

void CubeFace::CalculateVerts(TArray<FVector>* verts, TArray<FVector>* Normals, int steps)
{
	for (int y = 0, i = 0; y < steps; y++)
	{
		for (int x = 0; x < steps; x++, i++)
		{
			FVector vPosition = position / fullSize;
			float px = (float)x / (float)(steps - 1) * size / fullSize;
			float py = (float)y / (float)(steps - 1) * size / fullSize;
			vPosition += px * left + py * forward;

			if (shape)
			{
				vPosition = SpherifiedCube(vPosition);
			}
			if (noiseScale != 0)
			{				
				float height = perlinNoise->SimplexNoise3D(vPosition.X*noiseOffset, vPosition.Y*noiseOffset, vPosition.Z*noiseOffset) * noiseScale;

				(*verts)[i] = vPosition * (fullSize + height);
			}
			else
			{
				(*verts)[i] = vPosition * fullSize;
			}
		
			(*Normals)[i] = vPosition;
		}
	}
}

void CubeFace::RepairLOD(TArray<FVector>* verts, int steps)
{
	for (int y = 0, i = 0; y < steps; y++)
	{
		for (int x = 0; x < steps; x++, i++)
		{
			if (y == 0 && x % 2 == 1 || y == (steps - 1) && x % 2 == 1)
			{
				(*verts)[i] = FMath::Lerp((*verts)[i - 1], (*verts)[i + 1], 0.5f);
			}
			if (y % 2 == 1 && x == 0 || y % 2 == 1 && x == (steps - 1))
			{
				(*verts)[i] = FMath::Lerp((*verts)[i - (steps)], (*verts)[i + (steps)], 0.5f);
			}
		}
	}
}

void CubeFace::CalculateTris(TArray<int32>* tris, int steps)
{
	for (int ti = 0, vi = 0, y = 0; y < steps-1; y++, vi++)
	{
		for (int x = 0; x < steps-1; x++, ti += 6, vi++)
		{
			(*tris)[ti] = vi;
			(*tris)[ti + 4] = (*tris)[ti + 1] = vi + 1; // 3 2
			(*tris)[ti + 3] = (*tris)[ti + 2] = vi + (steps - 1) + 1; // 4 1
			(*tris)[ti + 5] = vi + (steps - 1) + 2;
		}
	}
}

FVector CubeFace::SpherifiedVector(FVector inVector)
{
	inVector /= fullSize;
	inVector = SpherifiedCube(inVector);
	if (perlinNoise && noiseScale != 0)
	{
		float height = perlinNoise->SimplexNoise3D(inVector.X*noiseOffset, inVector.Y*noiseOffset, inVector.Z*noiseOffset) * noiseScale;

		inVector *= (fullSize + height);
	}
	else
	{
		inVector *= fullSize;
	}	
	return inVector;
}

FVector CubeFace::SpherifiedCube(FVector p)
{
	FVector p2 = p * p;
	FVector res 
	(
		p.X * sqrt(1.0f - 0.5f * (p2.Z + p2.Y) + p2.Z*p2.Y / 3.0f),
		p.Y * sqrt(1.0f - 0.5f * (p2.Z + p2.X) + p2.Z*p2.X / 3.0f),
		p.Z * sqrt(1.0f - 0.5f * (p2.X + p2.Y) + p2.X*p2.Y / 3.0f)
	);
	return res;
}

UMaterialInstanceDynamic* CubeFace::CreateMaterial()
{
	UMaterialInstanceDynamic* mat = UMaterialInstanceDynamic::Create(material, mesh);
	FLinearColor color;
	color.R = FMath::FRandRange(0.3f, 1.0f);
	color.G = FMath::FRandRange(0.3f, 1.0f);
	color.B = FMath::FRandRange(0.3f, 1.0f);
	mat->SetVectorParameterValue("Color", color);
	return mat;
}

void CubeFace::UpdateVisible()
{
	if (needVisible > 30)
	{
		Hide();
	}
}

void CubeFace::ClearMesh()
{
	if (mesh)
	{
		mesh->ClearMeshSection(0);
		mesh->DestroyComponent();
		mesh = nullptr;
	}
	
	isGenerated = false;
	haveMesh = false;
}

void CubeFace::Dispose()
{	
	if(mesh)
		mesh->SetVisibility(false);
	isGenerated = false;
}

void CubeFace::Merge()
{
	if (!haveChild)
	{
		return;
	}

	for (CubeFace* face : childs)
	{
		face->Merge();
		face->ClearMesh();
		face->isChecked = true;
	}

	childs.Empty();
	haveChild = false;
}

void CubeFace::Hide()
{
	if (mesh)
	{
		mesh->SetVisibility(false);
	}
}

void CubeFace::Show()
{
	if (mesh)
	{
		if (isGenerated)
		{
			mesh->SetVisibility(true);
		}		
	}
}

TArray<CubeFace*> CubeFace::SubDivide()
{
	FVector ne = position + forward * (size / 2);
	childs.Add(new CubeFace(this, ne, left, forward, size / 2, depth + 1));

	FVector nw = position + forward * (size / 2) + left * (size / 2);
	childs.Add(new CubeFace(this, nw, left, forward, size / 2, depth + 1));

	FVector se = position;
	childs.Add(new CubeFace(this, se, left, forward, size / 2, depth + 1));

	FVector sw = position + left * (size / 2);
	childs.Add(new CubeFace(this, sw, left, forward, size / 2, depth + 1));

	haveChild = true;
	isChecked = false;

	Dispose();

	return childs;
}
