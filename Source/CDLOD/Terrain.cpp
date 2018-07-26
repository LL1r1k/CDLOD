#include "Terrain.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

ATerrain::ATerrain()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereTransform = CreateDefaultSubobject<USphereComponent>(FName("Transform"));
	SphereTransform->SetSphereRadius(30);
	SetRootComponent(SphereTransform);
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();
	ResetActors();

	player = GetWorld()->GetFirstPlayerController()->GetPawn();
	PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

	if (!material || !player)
	{
		GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		return;
	}

	Planet.Empty();

	Planet.SetNum(6);

	APlanet* Face1 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face1->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	Face1->InitializeStatic(Density, CubeSize, material, Shaped, BaseColor);
	Face1->InitializeNoise(NoiseOffset, NoiseScale, NoiseSeed, NoiseOctaves, NoiseLucanarity, NoiseGain);

	pos = FVector(-CubeSize, -CubeSize, -CubeSize);
	left = FVector(2, 0, 0);
	forward = FVector(0, 2, 0);
	Face1->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[0] = Face1;

	APlanet* Face2 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face2->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	pos = FVector(-CubeSize, CubeSize, -CubeSize);
	left = FVector(0, 0, 2);
	forward = FVector(0, -2, 0);
	Face2->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[1] = Face2;

	APlanet* Face3 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face3->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	pos = FVector(CubeSize, -CubeSize, -CubeSize);
	left = FVector(0, 0, 2);
	forward = FVector(0, 2, 0);
	Face3->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[2] = Face3;

	APlanet* Face4 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face4->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	pos = FVector(-CubeSize, -CubeSize, -CubeSize);
	left = FVector(0, 0, 2);
	forward = FVector(2, 0, 0);
	Face4->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[3] = Face4;

	APlanet* Face5 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face5->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	pos = FVector(CubeSize, CubeSize, -CubeSize);
	left = FVector(0, 0, 2);
	forward = FVector(-2, 0, 0);
	Face5->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[4] = Face5;

	APlanet* Face6 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
	Face6->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

	pos = FVector(CubeSize, -CubeSize, CubeSize);
	left = FVector(-2, 0, 0);
	forward = FVector(0, 2, 0);
	Face6->Initialize(pos, left, forward, CubeSize, 0, nullptr);
	Planet[5] = Face6;
}

void ATerrain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Planet.Num() > 0)
	{
		for (APlanet* face : Planet)
		{
			face->Merge(nullptr);
			face->DeleteActor();
		}
		Planet.Empty();
	}
	ResetActors();
	Super::EndPlay(EndPlayReason);
}

void ATerrain::OnConstruction(const FTransform& Transform)
{
	if (BaseColor.Num() != MaxDepth)
	{
		BaseColor.Empty();
		for (int i = 0; i < MaxDepth; i++)
		{
			BaseColor.Add(FColor::MakeRandomColor());
		}
	}
	if (ChangeSeed)
	{
		ChangeSeed = false;
		NoiseSeed = FMath::Rand();
		GenerateSphere = true;
	}
	if (GenerateSphere)
	{
		if (!material)
		{
			return;
		}

		GenerateSphere = false;	

		ResetActors();

		Planet.Empty();

		Planet.AddUninitialized(6);

		APlanet* Face1 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face1->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		Face1->InitializeStatic(Density, CubeSize, material, Shaped, BaseColor);
		Face1->InitializeNoise(NoiseOffset, NoiseScale, NoiseSeed, NoiseOctaves, NoiseLucanarity, NoiseGain);

		pos = FVector(-CubeSize, -CubeSize, -CubeSize);
		left = FVector(2, 0, 0);
		forward = FVector(0, 2, 0);
		Face1->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[0] = Face1;

		APlanet* Face2 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face2->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		pos = FVector(-CubeSize, CubeSize, -CubeSize);
		left = FVector(0, 0, 2);
		forward = FVector(0, -2, 0);
		Face2->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[1] = Face2;

		APlanet* Face3 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face3->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		pos = FVector(CubeSize, -CubeSize, -CubeSize);
		left = FVector(0, 0, 2);
		forward = FVector(0, 2, 0);
		Face3->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[2] = Face3;

		APlanet* Face4 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face4->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		pos = FVector(-CubeSize, -CubeSize, -CubeSize);
		left = FVector(0, 0, 2);
		forward = FVector(2, 0, 0);
		Face4->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[3] = Face4;

		APlanet* Face5 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face5->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		pos = FVector(CubeSize, CubeSize, -CubeSize);
		left = FVector(0, 0, 2);
		forward = FVector(-2, 0, 0);
		Face5->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[4] = Face5;

		APlanet* Face6 = GetWorld()->SpawnActor<APlanet>(FActorSpawnParameters());
		Face6->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		pos = FVector(CubeSize, -CubeSize, CubeSize);
		left = FVector(-2, 0, 0);
		forward = FVector(0, 2, 0);
		Face6->Initialize(pos, left, forward, CubeSize, 0, nullptr);
		Planet[5] = Face6;

		newposition = GetActorLocation();
		for (int i = 0; i < Planet.Num(); i++)
		{
			Update(i, true);
		}
	}
	else
	{
		ResetActors();
	}
	if (DeleteSphere)
	{
		DeleteSphere = false;
		ResetActors();
	}	
}

void ATerrain::ResetActors()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlanet::StaticClass(), FoundActors);
	for (AActor* actor : FoundActors)
	{
		APlanet* plan = (APlanet*)actor;
		plan->DeleteActor();
	}
	
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (generate)
	{
		newposition = player->GetActorLocation();
		for (int i = 0; i < Planet.Num(); i++)
		{
			Update(i, false);
		}
	}
}

void ATerrain::GetActiveQuadTreeFaces(APlanet* face, TArray<APlanet*>* activeTree)
{
	if (!face->haveChild)
	{
		activeTree->Add(face);
		face->isChecked = false;
	}
	else
		for (int i = 0; i < face->childs.Num(); i++)
			GetActiveQuadTreeFaces(face->childs[i], activeTree);
}

float ATerrain::ClosestDistance(FVector point, APlanet* poly, FVector *Closestpoint)
{
	float dist;
	FVector A, B, C, D;
	A = poly->A;
	B = poly->B;
	C = poly->C;
	D = poly->D;

	FVector closestpoint1 = FMath::ClosestPointOnTriangleToPoint(newposition, A, B, D);
	float dist1 = FVector::Dist(closestpoint1, newposition);

	FVector closestpoint2 = FMath::ClosestPointOnTriangleToPoint(newposition, A, C, D);
	float dist2 = FVector::Dist(closestpoint2, newposition);

	if (dist2 < dist1)
	{
		dist = dist2;
		*Closestpoint = closestpoint2;
	}
	else
	{
		dist = dist1;
		*Closestpoint = closestpoint1;
	}	

	return dist;
}

void ATerrain::SpherifiedCube(FVector* p)
{
	FVector p2 = *p * (*p);
	*p = FVector
	(
		p->X * sqrt(1.0f - 0.5f * (p2.Z + p2.Y) + p2.Z*p2.Y / 3.0f),
		p->Y * sqrt(1.0f - 0.5f * (p2.Z + p2.X) + p2.Z*p2.X / 3.0f),
		p->Z * sqrt(1.0f - 0.5f * (p2.X + p2.Y) + p2.X*p2.Y / 3.0f)
	);
}

void ATerrain::Update(int face, bool first)
{
	TArray<APlanet*> activeTree;

	GetActiveQuadTreeFaces(Planet[face], &activeTree);

	for (int i = 0; i < activeTree.Num(); i++)
	{
		if (!activeTree[i]->isChecked)
		{

			if (CameraCulling && !first)
			{
				if (!InCameraFrustum(activeTree[i]->A, activeTree[i]->B, activeTree[i]->C, activeTree[i]->D, 0))
				{
					activeTree[i]->needVisible = activeTree[i]->needVisible + 1;
				}
				else
				{
				activeTree[i]->Show();
				activeTree[i]->needVisible = 0;
				}
			}

			FVector closestpoint;
			float closestDist = ClosestDistance(newposition, activeTree[i], &closestpoint);
			float curSize = closestDist / (activeTree[i]->size);

			if (curSize < constant)
			{
				if (activeTree[i]->depth < MaxDepth)
				{
					activeTree.Append(activeTree[i]->SubDivide());
					activeTree[i]->needUpdate = false;
				}
			}
			else
			{
				if (activeTree[i]->parent != nullptr)
				{
					FVector par_closestpoint;
					float  par_closestDist = ClosestDistance(newposition, activeTree[i]->parent, &par_closestpoint);
					float par_curSize = par_closestDist / (activeTree[i]->parent->size);

					if (par_curSize > constant)
					{
						activeTree[i]->parent->Merge(&activeTree);
						activeTree[i]->parent->Generate();
					}
					else
					{
						activeTree[i]->Merge(nullptr);
						activeTree[i]->Generate();
					}
				}
				else
				{
					activeTree[i]->Merge(nullptr);
					activeTree[i]->Generate();
				}
		
			}
		}
		activeTree[i]->UpdateVisible();
	}

	for (int i = 0; i < activeTree.Num(); i++)
	{
		if(activeTree.IsValidIndex(i) && activeTree[i]->needUpdate)
			activeTree[i]->RepaitLOD(&activeTree);
	}

	activeTree.Empty();
}

bool ATerrain::InCameraFrustum(FVector WorldLocation)
{
	FVector2D ScreenLocation;

	if (!ProjectWorldToScren(PlayerController->Player, WorldLocation, ScreenLocation))
	{
		return false;
	}
	
	int32 ScreenX = (int32)ScreenLocation.X;
	int32 ScreenY = (int32)ScreenLocation.Y;

	int32 StartX = -ScreenWidth / 16;
	int32 StartY = -ScreenHeight / 16;
	
	return ScreenX > StartX && ScreenY > StartY && ScreenX < (ScreenWidth-StartX) && ScreenY < (ScreenHeight - StartY);
}

bool ATerrain::InCameraFrustum(FVector A, FVector B, FVector C, FVector D, int tolerances)
{	
	if (tolerances == 0)
	{
		if (InCameraFrustum(A) || InCameraFrustum(B) || InCameraFrustum(C) || InCameraFrustum(D))
		{

			if (!IsObjectOnTop(newposition, A) || !IsObjectOnTop(newposition, B) || !IsObjectOnTop(newposition, C) || !IsObjectOnTop(newposition, D))
			{
				return true;
			}
			else
			{
				return false;
			}	
			
		}
		else
		{
			return false;
		}
	}
	else
	{
		tolerances = tolerances - 1;
		if (tolerances < 2)
		{
			tolerances = 2;
		}
		// A -> B
		for (int i = 0; i < tolerances; i++)
		{
			float v = (float)i / (float)(tolerances - 1.0f);
			FVector cheak = FMath::Lerp(A, B, v);
			if (InCameraFrustum(cheak))
			{
				return true;
			}
		}

		// B -> D
		for (int i = 0; i < tolerances; i++)
		{
			float v = (float)i / (float)(tolerances - 1.0f);
			FVector cheak = FMath::Lerp(B, D, v);
			if (InCameraFrustum(cheak))
			{
				return true;
			}
		}

		// D -> C
		for (int i = 0; i < tolerances; i++)
		{
			float v = (float)i / (float)(tolerances - 1.0f);
			FVector cheak = FMath::Lerp(D, C, v);
			if (InCameraFrustum(cheak))
			{
				return true;
			}
		}

		// C -> A
		for (int i = 0; i < tolerances; i++)
		{
			float v = (float)i / (float)(tolerances - 1.0f);
			FVector cheak = FMath::Lerp(C, A, v);
			if (InCameraFrustum(cheak))
			{
				return true;
			}
		}

		return false;
	}	
}

bool ATerrain::ProjectWorldToScren(UPlayer* Player, FVector WorldLocation, FVector2D& ScreenLocation)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer != NULL && LocalPlayer->ViewportClient != NULL && LocalPlayer->ViewportClient->Viewport != NULL)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			LocalPlayer->GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags)
			.SetRealtimeUpdate(true));

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);

		if (SceneView)
		{
			FIntPoint size = LocalPlayer->ViewportClient->Viewport->GetSizeXY();
			int ClipX = size.X;
			int ClipY = size.Y;

			FPlane V = SceneView->Project(WorldLocation);
			FVector resultVec(V);
			resultVec.X = (ClipX / 2.f) + (resultVec.X*(ClipX / 2.f));
			resultVec.Y *= -1.f * GProjectionSignY;
			resultVec.Y = (ClipY / 2.f) + (resultVec.Y*(ClipY / 2.f));

			if (V.W <= 0.0f)
			{
				resultVec.Z = 0.0f;
				return false;
			}

			ScreenLocation.X = resultVec.X;
			ScreenLocation.Y = resultVec.Y;

			return true;
		}
	}

	return false;
}

bool ATerrain::IsObjectOnTop(FVector start, FVector object)
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = false;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(player);

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, start, FMath::Lerp(start,object,0.7f), ECC_WorldDynamic, TraceParams);
	if (FVector::Dist(start,object) < 1000.0f)
	{
		return false;
	}
	else
	{
		return Hit.bBlockingHit;
	}
	
}