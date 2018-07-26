#include "Planet.h"
#include "RuntimeMeshLibrary.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White,text)

static UMaterialInterface* material = nullptr;
static int density = 1;
static float fullSize = 100;
static bool shape = false;
static TArray<FColor> baseColor;

static float noiseOffset = 1;
static float noiseScale = 1;
static int32 noiseSeed = 0;
static int octaves = 1;
static float lucanarity = 2;
static float gain = 0.5f;

APlanet::APlanet()
{
	PrimaryActorTick.bCanEverTick = false;
	transform = CreateDefaultSubobject<USphereComponent>(FName("Transform"));
	transform->SetSphereRadius(30);
	SetRootComponent(transform);
	needBorder.AddUninitialized(4);
	for (int i = 0; i < 3; i++)
	{
		needBorder[i] = true;
	}
}

void APlanet::InitializeStatic(int dens, float size, UMaterialInterface* mat, bool shaped, TArray<FColor> color)
{
	density = dens;
	fullSize = size;
	material = mat;
	shape = shaped;
	material = mat;
	baseColor = color;
}

void APlanet::InitializeNoise(float noiseOff, float noiseScl, int32 noiseseed, int octav, float lucanar, float gai)
{
	noiseOffset = noiseOff;
	noiseScale = noiseScl;
	noiseSeed = noiseseed;
	octaves = octav;
	lucanarity = lucanar;
	gain = gai;
}

void APlanet::Initialize(FVector pos, FVector left, FVector forward, float size, int depth, APlanet* parent = nullptr)
{
	if(parent)
		this->parent = parent;
	position = pos;
	this->left = left;
	this->forward = forward;
	this->size = size;
	plane = new PerlinNoise();
	if (noiseSeed != 0)
	{
		plane->setNoiseSeed(noiseSeed);
	}
	if (shape)
	{

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

void APlanet::Destuctor()
{
	if (childs.Num() > 0)
	{
		for (APlanet* face : childs)
		{
			face->Destuctor();
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
	if (plane)
	{
		delete plane;
		plane = nullptr;
	}
}

void APlanet::ShowNieghbours()
{
/*	if (needBorder[0])*/
		DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[0]->A, Nieghbours[0]->D, 0.5f), FColor::Green, false, 10.0f);
	/*if (needBorder[1])*/
		DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[1]->A, Nieghbours[1]->D, 0.5f), FColor::Green, false, 10.0f);
	/*if (needBorder[2])*/
		DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[2]->A, Nieghbours[2]->D, 0.5f), FColor::Green, false, 10.0f);
	/*if (needBorder[3])*/
		DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[3]->A, Nieghbours[3]->D, 0.5f), FColor::Green, false, 10.0f);
}

bool APlanet::UpdateNieghbours()
{	
	if (necessUpdate)
	{
		necessUpdate = false;
		return true;
	}
		
	bool isHit = true;
	FCollisionShape MySphere = FCollisionShape::MakeSphere(10.0f);	
	FVector cros = FVector::CrossProduct(forward, left);
	cros = -cros;

	needBorder.Reset(0);
	needBorder.AddUninitialized(4);
	Nieghbours.Reset(0);
	Nieghbours.AddUninitialized(4);
	for (int i = 0; i < 4; i++)
	{
		needBorder[i] = false;
	}

	for (int i = 0; i < 4; i++)
	{
		Nieghbours[i] = nullptr;
	}	

	if (true/*DOWN*/)
	{
		FHitResult Hit;
		FVector hitPosition = position + forward * 4 * size / density - left * 4 * size / density;
		if (hitPosition.GetMax() > fullSize || hitPosition.GetMin() < -fullSize)
		{
			hitPosition += left * 4 * size / density;
			hitPosition += cros * 4 * size / density;
		}			
		
		if (shape)
			hitPosition = SpherifiedVector(hitPosition);
		GetWorld()->SweepSingleByChannel(Hit, hitPosition, hitPosition, FQuat::Identity, ECC_WorldStatic, MySphere);
		if (Hit.bBlockingHit)
		{
			APlanet* neigh = Cast<APlanet>(Hit.Actor);
			if (neigh)
			{
				if (!isGenerated)
				{
					return false;
				}
				Nieghbours[0] = neigh;
				if (neigh->depth < depth)
				{
					needBorder[0] = true;
				}
			}
			else
			{
				isHit = false;
				DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red);
				return isHit;
			}			
		}
		else
		{
			isHit = false;
			//DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red, false , 10.0f);
			return isHit;
		}
	}
	
	if (true/*LEFT*/)
	{
		FHitResult Hit;
		FVector hitPosition = position + forward * size + left * size / density * 4 + forward * size / density * 4;
		if (hitPosition.GetMax() > fullSize || hitPosition.GetMin() < -fullSize)
		{
			hitPosition -= forward * size / density * 4;
			hitPosition += cros * 4 * size / density;
		}

		if(shape)
			hitPosition = SpherifiedVector(hitPosition);
		GetWorld()->SweepSingleByChannel(Hit, hitPosition, hitPosition, FQuat::Identity, ECC_WorldStatic, MySphere);
		if (Hit.bBlockingHit)
		{
			APlanet* neigh = Cast<APlanet>(Hit.Actor);
			if (neigh)
			{
				if (!isGenerated)
				{
					return false;
				}
				Nieghbours[1] = neigh;
				if (neigh->depth < depth)
				{
					needBorder[1] = true;
				}
			}
			else
			{
				isHit = false;
				DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red);
				return isHit;
			}			
		}
		else
		{
			isHit = false;
			//DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red, false, 10.0f);
			return isHit;
		}
	}
	
	if (true/*UP*/)
	{
		FHitResult Hit;
		FVector hitPosition = position + left * size + forward * 4 * size / density + left * 4 * size / density;
		if (hitPosition.GetMax() > fullSize || hitPosition.GetMin() < -fullSize)
		{			
			hitPosition -= left * 4 * size / density;
			hitPosition += cros * 4 * size / density;
		}
		if (shape)
			hitPosition = SpherifiedVector(hitPosition);
		GetWorld()->SweepSingleByChannel(Hit, hitPosition, hitPosition, FQuat::Identity, ECC_WorldStatic, MySphere);
		if (Hit.bBlockingHit)
		{
			APlanet* neigh = Cast<APlanet>(Hit.Actor);
			if (neigh)
			{
				if (!isGenerated)
				{
					return false;
				}
				Nieghbours[2] = neigh;
				if (neigh->depth < depth)
				{
					needBorder[2] = true;
				}
			}
			else
			{
				isHit = false;
				DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red);
				return isHit;
			}			
		}
		else
		{
			isHit = false;
			//DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red, false, 10.0f);
			return isHit;
		}
	}
	
	if (true/*RIGHT*/)
	{
		FHitResult Hit;
		FVector hitPosition = position + left * 4 * size / density - forward * 4 * size / density;
		if (hitPosition.GetMax() > fullSize || hitPosition.GetMin() < -fullSize)
		{
			hitPosition += forward * 4 * size / density;
			hitPosition += cros * 4 * size / density;
		}
		if (shape)
			hitPosition = SpherifiedVector(hitPosition);
		GetWorld()->SweepSingleByChannel(Hit, hitPosition, hitPosition, FQuat::Identity, ECC_WorldStatic, MySphere);
		if (Hit.bBlockingHit)
		{
			APlanet* neigh = Cast<APlanet>(Hit.Actor);
			if (neigh)
			{
				if (!isGenerated)
				{
					return false;
				}
				Nieghbours[3] = neigh;
				if (neigh->depth < depth)
				{
					needBorder[3] = true;
				}
			}
			else
			{
				isHit = false;
				DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red);
				return isHit;
			}			
		}
		else
		{
			isHit = false;
			//DrawDebugPoint(GetWorld(), hitPosition, 20.0f, FColor::Red, false, 10.0f);
			return isHit;
		}
	}	
	return isHit;
}	

void APlanet::Generate()
{	
	if (isGenerated)
	{
		Show();
		return;
	}	

	if (haveMesh)
	{
		Show();
		isGenerated = true;
		return;
	}

	if (!mesh)
	{
		mesh = NewObject<URuntimeMeshComponent>(this);
		mesh->RegisterComponent();
	}	

	int steps = density + 1;
	verticesPerLine = density + 1;

	Vertices.Empty();
	Normals.Empty();
	BorderVertices.Empty();
	BorderTriangles.Empty();
	vertexIndexMap.Empty();
	Triangles.Empty();
	UVs.Empty();

	Vertices.AddUninitialized(verticesPerLine * verticesPerLine);
	BorderVertices.AddUninitialized(verticesPerLine * 4 + 4);
	Normals.AddUninitialized(verticesPerLine * verticesPerLine);
	UVs.AddUninitialized(verticesPerLine * verticesPerLine);	

	CalculateVerts();
	CalculateNormals();
	//URuntimeMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, Normals, UVs, Tangents, true);

	mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColor, Tangents, true, EUpdateFrequency::Infrequent);
	if (material)
	{
		mesh->SetMaterial(0, CreateMaterial());
	}

	isGenerated = true;
	haveMesh = true;	
	Show();
}

void APlanet::UpdateMesh()
{
	if (mesh == nullptr || !isGenerated)
		return;  

	bool isChanged = false;

	int upIndex = 0;
	int downIndex = 0;
	int rightIndex = 0;
	int leftIndex = 0;

	bool needUpBorder = true;
	bool needDownBorder = true;
	bool needRightBorder = true;
	bool needLeftBorder = true;

	for (int y = 0, i = 0; y < verticesPerLine + 2; y++)
	{
		for (int x = 0; x < verticesPerLine + 2; x++, i++)
		{
			int vertexIndex = vertexIndexMap[i];
			if (needLeftBorder)
			{
				bool isLeftBorder = x % 2 == 0 && y == verticesPerLine && x > 0 && x < verticesPerLine + 1;
				if (isLeftBorder)
				{
					if (needBorder[1])
					{
						FVector nowVector = Vertices[vertexIndex];
						FVector middleVector = FMath::Lerp(Vertices[vertexIndex + 1], Vertices[vertexIndex - 1], 0.5f);
						if (nowVector != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = middleVector;
						}
						else
						{
							needLeftBorder = false;
						}
					}
					else
					{
						FVector middleVector = Vertices[vertexIndex];
						if (LeftBorder[leftIndex] != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = LeftBorder[leftIndex];
						}
						else
						{
							needLeftBorder = false;
						}
					}
					leftIndex++;
				}
			}

			if (needRightBorder)
			{
				bool isRightBorder = x % 2 == 0 && y == 1 && x > 0 && x < verticesPerLine + 1;
				if (isRightBorder)
				{
					if (needBorder[3])
					{
						FVector nowVector = Vertices[vertexIndex];
						FVector middleVector = FMath::Lerp(Vertices[vertexIndex + 1], Vertices[vertexIndex - 1], 0.5f);
						if (nowVector != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = middleVector;
						}
						else
						{
							needRightBorder = false;
						}
					}
					else
					{
						FVector middleVector = Vertices[vertexIndex];
						if (RightBorder[rightIndex] != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = RightBorder[rightIndex];
						}
						else
						{
							needRightBorder = false;
						}
					}
					rightIndex++;
				}
			}

			if (needDownBorder)
			{
				bool isDownBorder = y % 2 == 0 && x == 1 && y > 0 && y < verticesPerLine + 1;
				if (isDownBorder)
				{
					if (needBorder[0])
					{
						FVector nowVector = Vertices[vertexIndex];
						FVector middleVector = FMath::Lerp(Vertices[vertexIndexMap[i + verticesPerLine + 2]], Vertices[vertexIndexMap[i - verticesPerLine - 2]], 0.5f);
						if (nowVector != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = middleVector;
						}
						else
						{
							needDownBorder = false;
						}
					}
					else
					{
						FVector middleVector = Vertices[vertexIndex];
						if (DownBorder[downIndex] != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = DownBorder[downIndex];
						}
						else
						{
							needDownBorder = false;
						}
					}
					downIndex++;
				}
			}

			if (needUpBorder)
			{
				bool isUpBorder = y % 2 == 0 && x == verticesPerLine && y > 0 && y < verticesPerLine + 1;
				if (isUpBorder)
				{
					if (needBorder[2])
					{
						FVector nowVector = Vertices[vertexIndex];
						FVector middleVector = FMath::Lerp(Vertices[vertexIndexMap[i + verticesPerLine + 2]], Vertices[vertexIndexMap[i - verticesPerLine - 2]], 0.5f);
						if (nowVector != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = middleVector;
						}
						else
						{
							needUpBorder = false;
						}
					}
					else
					{
						FVector middleVector = Vertices[vertexIndex];
						if (UpBorder[upIndex] != middleVector)
						{
							isChanged = true;
							Vertices[vertexIndex] = UpBorder[upIndex];
						}
						else
						{
							needUpBorder = false;
						}
					}
					upIndex++;
				}
			}
		}
	}
	if (isChanged)
			mesh->UpdateMeshSection(0, Vertices, Normals, UVs, VertexColor, Tangents);
}

void APlanet::AddVertex(FVector VertexPosition, FVector2D UV, int vertexIndex)
{
	if (vertexIndex < 0)
	{
		BorderVertices[-vertexIndex - 1] = VertexPosition;
	}
	else
	{
		Vertices[vertexIndex] = VertexPosition;
		UVs[vertexIndex] = UV;
	}
}

void APlanet::AddTriangle(int a, int b, int c)
{
	if (a < 0 || b < 0 || c < 0)
	{
		BorderTriangles.Add(a);
		BorderTriangles.Add(b);
		BorderTriangles.Add(c);
	}
	else
	{
		Triangles.Add(a);
		Triangles.Add(b);
		Triangles.Add(c);
	}
	
}

void APlanet::CalculateVerts()
{
	FVector borderStart = position - left * size / density - forward * size / density;
	float SizeWithBorder = size + 2 * (size / density);

	vertexIndexMap.AddUninitialized((verticesPerLine + 2) * (verticesPerLine + 2));
	int meshVertexIndex = 0;
	int borderVertexIndex = -1;

	for (int y = 0, i = 0; y < verticesPerLine + 2; y++)
	{
		for (int x = 0; x < verticesPerLine + 2; x++, i++)
		{			
			bool isBorderVertex = x == 0 || y == 0 || x == verticesPerLine + 1 || y == verticesPerLine + 1;

			if (isBorderVertex)
			{
				vertexIndexMap[i] = borderVertexIndex;
				borderVertexIndex--;
			}
			else
			{
				vertexIndexMap[i] = meshVertexIndex;
				meshVertexIndex++;
			}
		}
	}	

	for (int y = 0, i = 0; y < verticesPerLine + 2; y++)
	{
		for (int x = 0; x < verticesPerLine + 2; x++, i++)
		{
			int vertexIndex = vertexIndexMap[i];
			FVector vPosition = borderStart;
			float percentX = (float)x / (float)(verticesPerLine - 1 + 2) * SizeWithBorder;
			float percentY = (float)y / (float)(verticesPerLine - 1 + 2) * SizeWithBorder;
			vPosition += percentX * left + percentY * forward;
			bool isBorderVertex = x == 0 || y == 0 || x == verticesPerLine + 1 || y == verticesPerLine + 1;
			if (isBorderVertex && (vPosition.GetMax() > fullSize || vPosition.GetMin() < -fullSize))
			{	
				if ((vPosition*left).GetMax() > fullSize)
				{
					vPosition += -left * size / density;
				}
				if ((vPosition*left).GetMin() < -fullSize)
				{
					vPosition += left * size / density;
				}
				if ((vPosition*forward).GetMax() > fullSize)
				{
					vPosition += -forward * size / density;
				}
				if ((vPosition*forward).GetMin() < -fullSize)
				{
					vPosition += forward * size / density;
				}

				FVector normal = FVector::CrossProduct(left, forward);
				vPosition += normal / 2 * size / density;
			}	

			vPosition /= fullSize;

			if (shape)
				vPosition = Spherified(vPosition);

			if (noiseScale != 0)
			{
				float height = noise(vPosition*noiseOffset) * noiseScale;
				vPosition *= (fullSize + height);
			}
			else
			{
				vPosition *= fullSize;
			}
			float uvFactor = 1.0f / size;

			if (!LeftBorderCreated)
			{
				bool isLeftBorder = x % 2 == 0 && y == verticesPerLine && x > 0 && x < verticesPerLine + 1;				
				if (isLeftBorder)
				{
					LeftBorder.Add(vPosition);
				}
			}	

			if (!RightBorderCreated)
			{
				bool isRightBorder = x % 2 == 0 && y == 1 && x > 0 && x < verticesPerLine + 1;
				if (isRightBorder)
				{
					RightBorder.Add(vPosition);
				}
			}

			if (!UpBorderCreated)
			{
				bool isUpBorder = y % 2 == 0 && x == verticesPerLine && y > 0 && y < verticesPerLine + 1;
				if (isUpBorder)
				{
					UpBorder.Add(vPosition);
				}
			}

			if (!DownBorderCreated)
			{
				bool isDownBorder = y % 2 == 0 && x == 1 && y > 0 && y < verticesPerLine + 1;
				if (isDownBorder)
				{
					DownBorder.Add(vPosition);
				}
			}

			AddVertex(vPosition, FVector2D((float)x * uvFactor, (float)y *uvFactor), vertexIndex);

			if (x < verticesPerLine + 1 && y < verticesPerLine + 1)
			{
				int a = vertexIndexMap[i];
				int b = vertexIndexMap[i + 1];
				int c = vertexIndexMap[i + verticesPerLine + 2];
				int d = vertexIndexMap[i + verticesPerLine + 3];

				AddTriangle(a, d, c);
				AddTriangle(d, a, b);
			}
		}
	}

	LeftBorderCreated = true;
	RightBorderCreated = true;
	DownBorderCreated = true;
	UpBorderCreated = true;

}

void APlanet::CalculateTris()
{
	for (int ti = 0, vi = 0, y = 0; y < verticesPerLine - 1; y++, vi++)
	{
		for (int x = 0; x < verticesPerLine - 1; x++, ti += 6, vi++)
		{
			Triangles[ti] = vi;
			Triangles[ti + 4] = Triangles[ti + 1] = vi + 1; // 3 2
			Triangles[ti + 3] = Triangles[ti + 2] = vi + (verticesPerLine - 1) + 1; // 4 1
			Triangles[ti + 5] = vi + (verticesPerLine - 1) + 2;
		}
	}
}

void APlanet::CalculateNormals()
{
	int triangleCount = Triangles.Num() / 3;
	for (int i = 0; i < Normals.Num(); i++)
	{
		Normals[i] = FVector(0, 0, 0);
	}
	for (int i = 0; i < triangleCount; i++)
	{
		int normalTriangleIndex = i * 3;
		int vertexIndexA = Triangles[normalTriangleIndex];
		int vertexIndexB = Triangles[normalTriangleIndex + 1];
		int vertexIndexC = Triangles[normalTriangleIndex + 2];

		FVector triangleNormal = SurfaceNormalFromIndices(vertexIndexA, vertexIndexB, vertexIndexC);
		Normals[vertexIndexA] += triangleNormal;
		Normals[vertexIndexB] += triangleNormal;
		Normals[vertexIndexC] += triangleNormal;
	}

	int borderTriangleCount = BorderTriangles.Num() / 3;
	for (int i = 0; i < borderTriangleCount; i++)
	{
		int normalTriangleIndex = i * 3;
		int vertexIndexA = BorderTriangles[normalTriangleIndex];
		int vertexIndexB = BorderTriangles[normalTriangleIndex + 1];
		int vertexIndexC = BorderTriangles[normalTriangleIndex + 2];

		FVector triangleNormal = SurfaceNormalFromIndices(vertexIndexA, vertexIndexB, vertexIndexC);
		if(vertexIndexA >= 0)
			Normals[vertexIndexA] += triangleNormal;
		if (vertexIndexB >= 0)
			Normals[vertexIndexB] += triangleNormal;
		if (vertexIndexC >= 0)
			Normals[vertexIndexC] += triangleNormal;
	}

	for (int i = 0; i < Normals.Num(); i++)
	{
		Normals[i].Normalize();
	}
}

void APlanet::RepaitLOD(TArray<APlanet*>* activeTree)
{
	if (Nieghbours.Num() < 1)
	{
		isUpdated = false;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (Nieghbours[i] == nullptr)
			{
				isUpdated = false;
				break;
			}
		}
	}	

	if (!haveMesh || isUpdated)
		return;

	if(UpdateNieghbours())
	{
		isUpdated = true;
		//ShowNieghbours();
		UpdateMesh();		
		
		int bottomIndex = 2;
		int leftIndex = 3;
		int upIndex = 0;
		int rightIndex = 1;
		if (Nieghbours[2]->Nieghbours.Num() > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (Nieghbours[2]->Nieghbours[i] == this)
					upIndex = i;
			}
		}
		if (Nieghbours[0]->Nieghbours.Num() > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (Nieghbours[0]->Nieghbours[i] == this)
					bottomIndex = i;
			}
		}
		if (Nieghbours[1]->Nieghbours.Num() > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (Nieghbours[1]->Nieghbours[i] == this)
					leftIndex = i;
			}
		}
		if (Nieghbours[3]->Nieghbours.Num() > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				if (Nieghbours[3]->Nieghbours[i] == this)
					rightIndex = i;
			}
		}		

		if (needBorder[0] != Nieghbours[0]->needBorder[bottomIndex] && depth == Nieghbours[0]->depth)
		{
			Nieghbours[0]->isUpdated = false;
			//DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[0]->A, Nieghbours[0]->D, 0.5f), FColor::Red, false, 10.0f);
		}		

		if (needBorder[1] != Nieghbours[1]->needBorder[leftIndex] && depth == Nieghbours[1]->depth)
		{
			Nieghbours[1]->isUpdated = false;
			//DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[1]->A, Nieghbours[1]->D, 0.5f), FColor::Red, false, 10.0f);
		}		

		if (needBorder[2] != Nieghbours[2]->needBorder[upIndex] && depth == Nieghbours[2]->depth)
		{
			Nieghbours[2]->isUpdated = false;
			//DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[2]->A, Nieghbours[2]->D, 0.5f), FColor::Red, false, 10.0f);
		}	

		if (needBorder[3] != Nieghbours[3]->needBorder[rightIndex] && depth == Nieghbours[3]->depth)
		{
			Nieghbours[3]->isUpdated = false;
			//DrawDebugLine(GetWorld(), FMath::Lerp(A, D, 0.5f), FMath::Lerp(Nieghbours[3]->A, Nieghbours[3]->D, 0.5f), FColor::Red, false, 10.0f);
		}
	}	
}

FVector APlanet::SurfaceNormalFromIndices(int indexA, int indexB, int indexC)
{
	FVector pointA = (indexA < 0) ? BorderVertices[-indexA - 1] : Vertices[indexA];
	FVector pointB = (indexB < 0) ? BorderVertices[-indexB - 1] : Vertices[indexB];
	FVector pointC = (indexC < 0) ? BorderVertices[-indexC - 1] : Vertices[indexC];

	FVector sideAB = pointB - pointA;
	FVector sideAC = pointC - pointA;
	FVector res = FVector::CrossProduct(sideAC, sideAB);

	return res;
}

UMaterialInstanceDynamic* APlanet::CreateMaterial()
{
	if (!baseMaterial)
	{
		baseMaterial = UMaterialInstanceDynamic::Create(material, mesh);
	}	
	FLinearColor color(baseColor[0]);
	baseMaterial->SetVectorParameterValue("Color", color);
	return baseMaterial;
}

FVector APlanet::SpherifiedVector(FVector inVector)
{
	inVector /= fullSize;
	inVector = Spherified(inVector);
	if (plane && noiseScale != 0)
	{
		float height = noise(inVector*noiseOffset) * noiseScale;

		inVector *= (fullSize + height);
	}
	else
	{
		inVector *= fullSize;
	}
	return inVector;
}

FVector APlanet::Spherified(FVector p)
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

void APlanet::UpdateVisible()
{
	if (needVisible > 30)
	{
		Hide();
	}
}

float APlanet::noise(FVector point)
{
	float amplitude = 1.0;
	float frequency = 1.0;
	float sum = 0.0;
	for (int i = 0; i < octaves; ++i)
	{
		sum += amplitude * plane->SimplexNoise3D(point.X * frequency, point.Y * frequency, point.Z * frequency);
		amplitude *= gain;
		frequency *= lucanarity;
	}
	return sum;
}

void APlanet::DeleteActor()
{
	if (mesh)
	{
		mesh->ClearMeshSection(0);
		mesh->DestroyComponent();
		mesh = nullptr;
	}

	if (plane)
	{
		delete plane;
		plane = nullptr;
	}

	Nieghbours.Empty();

	isGenerated = false;
	haveMesh = false;
	K2_DestroyActor();

}

void APlanet::Dispose()
{
	Hide();
	isGenerated = false;
}

void APlanet::Merge(TArray<APlanet*>* activeTree)
{
	if (!haveChild)
	{
		return;
	}

	for (APlanet* face : childs)
	{
		face->Merge(nullptr);
		/*if (activeTree != nullptr)
		{			
			if (face->Nieghbours.Num() > 0)
			{
				if (face->Nieghbours[0] != nullptr)
				{

					if (face->needBorder[0] == face->Nieghbours[0]->needBorder[2])
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[0]->A, face->Nieghbours[0]->D, 0.5f), FColor::Green, false, 10.0f);
						face->Nieghbours[0]->needBorder[2] = true;
						//face->Nieghbours[0]->ShowNieghbours();
						face->Nieghbours[0]->UpdateMesh();
					}
					if (face->Nieghbours[0]->needBorder[2] == false && face->depth < face->Nieghbours[0]->depth)
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[0]->A, face->Nieghbours[0]->D, 0.5f), FColor::Yellow, false, 10.0f);
						face->Nieghbours[0]->isUpdated = false;
					}
				}

				if (face->Nieghbours[1] != nullptr)
				{

					if (face->needBorder[1] == face->Nieghbours[1]->needBorder[3])
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[1]->A, face->Nieghbours[1]->D, 0.5f), FColor::Green, false, 10.0f);
						face->Nieghbours[1]->needBorder[3] = true;
						//	face->Nieghbours[1]->ShowNieghbours();
						face->Nieghbours[1]->UpdateMesh();
					}
					if (face->Nieghbours[1]->needBorder[3] == false && face->depth < face->Nieghbours[1]->depth)
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[1]->A, face->Nieghbours[1]->D, 0.5f), FColor::Yellow, false, 10.0f);
						face->Nieghbours[1]->isUpdated = false;
					}
				}

				if (face->Nieghbours[2] != nullptr)
				{

					if (face->needBorder[2] == face->Nieghbours[2]->needBorder[0])
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[2]->A, face->Nieghbours[2]->D, 0.5f), FColor::Green, false, 10.0f);
						face->Nieghbours[2]->needBorder[0] = true;
						//face->Nieghbours[2]->ShowNieghbours();
						face->Nieghbours[2]->UpdateMesh();
					}
					if (face->Nieghbours[2]->needBorder[0] == false && face->depth < face->Nieghbours[2]->depth)
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[2]->A, face->Nieghbours[2]->D, 0.5f), FColor::Yellow, false, 10.0f);
						face->Nieghbours[2]->isUpdated = false;
					}
				}

				if (face->Nieghbours[3] != nullptr)
				{

					if (face->needBorder[3] == face->Nieghbours[3]->needBorder[1])
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[3]->A, face->Nieghbours[3]->D, 0.5f), FColor::Green, false, 10.0f);
						face->Nieghbours[3]->needBorder[1] = false;
						//face->Nieghbours[3]->ShowNieghbours();
						face->Nieghbours[3]->UpdateMesh();
					}
					if (face->Nieghbours[3]->needBorder[1] == false && face->depth < face->Nieghbours[3]->depth)
					{
						//DrawDebugLine(GetWorld(), FMath::Lerp(face->A, face->D, 0.5f), FMath::Lerp(face->Nieghbours[3]->A, face->Nieghbours[3]->D, 0.5f), FColor::Yellow, false, 10.0f);
						face->Nieghbours[3]->isUpdated = false;
					}
				}
			}			
		}	*/
		if (face->Nieghbours.Num() > 0)
		{
			if (face->Nieghbours[0]->Nieghbours.Num() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					if (face->Nieghbours[0]->Nieghbours[i] == this)
						face->Nieghbours[0]->Nieghbours[i] = nullptr;
				}
			}
			
			if (face->Nieghbours[1]->Nieghbours.Num() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					if (face->Nieghbours[1]->Nieghbours[i] == this)
						face->Nieghbours[1]->Nieghbours[i] = nullptr;
				}
			}

			if (face->Nieghbours[2]->Nieghbours.Num() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					if (face->Nieghbours[2]->Nieghbours[i] == this)
						face->Nieghbours[2]->Nieghbours[i] = nullptr;
				}
			}

			if (face->Nieghbours[3]->Nieghbours.Num() > 0)
			{
				for (int i = 0; i < 4; i++)
				{
					if (face->Nieghbours[3]->Nieghbours[i] == this)
						face->Nieghbours[3]->Nieghbours[i] = nullptr;
				}
			}
		}
		face->needUpdate = false;
		face->DeleteActor();
		face->isChecked = true;
		face->isUpdated = false;
	}

	childs.Empty();
	isUpdated = false;
	needUpdate = true;
	haveChild = false;
}

void APlanet::Hide()
{
	if (mesh)
	{
		mesh->SetVisibility(false);
		mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void APlanet::Show()
{
	if (mesh)
	{
		if (isGenerated)
		{
			mesh->SetVisibility(true);
			mesh->SetCollisionResponseToAllChannels(ECR_Block);
		}
	}
}

TArray<APlanet*> APlanet::SubDivide()
{
	FVector ne = position + forward * (size / 2);
	APlanet* ne_part = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	ne_part->Initialize(ne, left, forward, size / 2, depth + 1, this);
	ne_part->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	ne_part->cheackniegh = true;
	childs.Add(ne_part); 

	FVector nw = position + forward * (size / 2) + left * (size / 2);
	APlanet* nw_part = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	nw_part->Initialize(nw, left, forward, size / 2, depth + 1, this);
	nw_part->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	nw_part->cheackniegh = true;
	childs.Add(nw_part);

	FVector se = position;
	APlanet* se_part = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	se_part->Initialize(se, left, forward, size / 2, depth + 1, this);
	se_part->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	se_part->cheackniegh = true;
	childs.Add(se_part); 

	FVector sw = position + left * (size / 2);
	APlanet* sw_part = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	sw_part->Initialize(sw, left, forward, size / 2, depth + 1, this);
	sw_part->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	sw_part->cheackniegh = true;
	childs.Add(sw_part);

	haveChild = true;
	isChecked = false;

	Dispose();

	return childs;
}

