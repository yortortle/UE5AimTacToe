// Fill out your copyright notice in the Description page of Project Settings.

#include "YortGridMesh.h"
#include "AimToeProjectile.h"

AYortGridMesh::AYortGridMesh() :
	bIsLerping(true),
	LerpAlpha(0.0f),
	LerpSpeed(0.0f)
{
	//Here we're force loading a cube mesh object to be used for our tic tac toe grid objects with FObjectFinder
	ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"), LOAD_Quiet | LOAD_NoWarn);
	if (!CubeFinder.Succeeded())
	{
		return;
	}

	// If we've found our cubemesh, we proceed to create 9 box components with our box component function, 
	// all of them created directly in our gridmap used to store our boxes with their corresponding BoxID's
	// The FVector Passed in for location determines their spot in the grid.
	UStaticMesh* CubeMesh = CubeFinder.Object;
	GridMap.Add(2, CreateBoxComponent(CubeMesh, FVector::ZeroVector, TEXT("BoxComp1"), 2, true));
	GridMap.Add(1, CreateBoxComponent(CubeMesh, FVector(-200, 0, 0), TEXT("BoxComp2"), 1));
	GridMap.Add(3, CreateBoxComponent(CubeMesh, FVector(200, 0, 0), TEXT("BoxComp3"), 3));
	GridMap.Add(4, CreateBoxComponent(CubeMesh, FVector(-200, 0, 200), TEXT("BoxComp4"), 4));
	GridMap.Add(5, CreateBoxComponent(CubeMesh, FVector(0, 0, 200), TEXT("BoxComp5"), 5));
	GridMap.Add(6, CreateBoxComponent(CubeMesh, FVector(200, 0, 200), TEXT("BoxComp6"), 6));
	GridMap.Add(7, CreateBoxComponent(CubeMesh, FVector(-200, 0, 400), TEXT("BoxComp7"), 7));
	GridMap.Add(8, CreateBoxComponent(CubeMesh, FVector(0, 0, 400), TEXT("BoxComp8"), 8));
	GridMap.Add(9, CreateBoxComponent(CubeMesh, FVector(200, 0, 400), TEXT("BoxComp9"), 9));
}

UAimToeBoxComponent* AYortGridMesh::CreateBoxComponent(UStaticMesh* MeshToUse, FVector Offset, FName ComponentName, int32 BoxID, bool bIsRootComponent)
{
	UAimToeBoxComponent* Box = CreateDefaultSubobject<UAimToeBoxComponent>(ComponentName);

	//setting the collision, static mesh instance to use, on event hit binding, and the box ID which will specifically be used later on for comparisons
	Box->BodyInstance.SetCollisionProfileName("Projectile");

	//binding a delegate function for the on hit of this box, handles our win condition logic
	Box->OnComponentHit.AddUniqueDynamic(this, &AYortGridMesh::OnHit);
	Box->SetStaticMesh(MeshToUse);
	Box->AddLocalOffset(Offset);
	Box->BoxID = BoxID;

	//We need the first box to be the root component which the others will follow the transform of.
	if (bIsRootComponent)
	{
		RootComponent = Box;
	}
	else
	{
		// In order to keep the boxes movement consistent in the actor to the other boxes, we attach it to the first box which is set as the root component, and it will follow the transform as the grid actor is moved through the scene.
		Box->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	return Box;
}

void AYortGridMesh::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (bIsLerping)
	{
		// if we're lerping, we iterate over our tic tac toe grid box components
		for (const TPair<int32, UAimToeBoxComponent*>& pair : GridMap)
		{
			// I generate a percentage value which will let us 
			// lerp back and forth using the sin value of LerpAlpha plus delta seconds times a value to decrease the overall distance.
			float LerpPercent = FMath::Sin(LerpAlpha += (DeltaSeconds * .5));
			FVector BoxLocation = pair.Value->GetComponentLocation();
			FVector NewLoc = FVector::ZeroVector;

			// The first box put into the map needs a special conditional with 200 extra distance added to it's X, so I add a check for it
			if (pair.Key == 2)
			{
				NewLoc = FMath::Lerp(FVector(BoxLocation.X, BoxLocation.Y, BoxLocation.Z), FVector(BoxLocation.X + (LerpSpeed + 200), BoxLocation.Y, BoxLocation.Z), LerpPercent * .05);
			}
			else
			{
				// I Lerp between the X value of the first box with the boxes same location with an added value called "LerpSpeed" which is selected by the designer in the editor.
				// The final value in the lerp is the percent which corresponds to what gets spit out of the sin function above
				NewLoc = FMath::Lerp(FVector(BoxLocation.X, BoxLocation.Y, BoxLocation.Z), FVector(BoxLocation.X + LerpSpeed, BoxLocation.Y, BoxLocation.Z), LerpPercent * .05);
			}

			pair.Value->SetWorldLocation(NewLoc);
		}
	}
}

void AYortGridMesh::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//the contents of this function should on run if the actor that hit it is an aim toe projectile
	AAimToeProjectile* proj = Cast<AAimToeProjectile>(OtherActor);
	if (!proj)
	{
		return;
	}

	//destroying projectectile that hit this box so that it doesn't bounce to other projectiles
	proj->Destroy();

	if (UAimToeBoxComponent* HitToeBoxComp = Cast<UAimToeBoxComponent>(HitComp))
	{
		//settings the boxes player id so that the box knows which player has hit it last
		HitToeBoxComp->PlayerID = proj->ProjectileID;

		// Allows designers to bind into the blueprint function in the editor to call their own custom logic that doesn't need to be in C++.
		// currently these functions are handling a material swap for the editor
		if (HitToeBoxComp->PlayerID == 0)
		{
			OnPlayerOneActivateBlock(HitComp);
		}
		else if (HitToeBoxComp->PlayerID == 1)
		{
			OnPlayerTwoActivateBlock(HitComp);
		}

		//Our tic tac toe win condition logic, uses two helper functions to keep the code clean and tidy
		if (CompareBoxes(1, 5) && CompareBoxes(5, 9))
		{
			WinCondition(1, 5, 9);
		}
		else if (CompareBoxes(3, 5) && CompareBoxes(5, 7))
		{
			WinCondition(3, 5, 7);
		}
		else if (CompareBoxes(1, 2) && CompareBoxes(2, 3))
		{
			WinCondition(1, 2, 3);
		}
		else if (CompareBoxes(4, 5) && CompareBoxes(5, 6))
		{
			WinCondition(4, 5, 6);
		}
		else if (CompareBoxes(7, 8) && CompareBoxes(8, 9))
		{
			WinCondition(7, 8, 9);
		}
		else if (CompareBoxes(1, 4) && CompareBoxes(4, 7))
		{
			WinCondition(1, 4, 7);
		}
		else if (CompareBoxes(2, 5) && CompareBoxes(5, 8))
		{
			WinCondition(2, 5, 8);
		}
		else if (CompareBoxes(3, 6) && CompareBoxes(6, 9))
		{
			WinCondition(3, 6, 9);
		}
	}
}

int32 AYortGridMesh::GetBoxPID(int32 BoxID)
{
	//find the box at the specified box ID and returns the player ID associated with it, aka who hit it last (player 1 or player 2)
	return (*GridMap.Find(BoxID))->PlayerID;
}

bool AYortGridMesh::CompareBoxes(int32 BoxIdOne, int32 BoxIdTwo)
{
	// we return false if the boxes are at their default value of -1
	if (GetBoxPID(BoxIdOne) == -1 || GetBoxPID(BoxIdTwo) == -1)
	{
		return false;
	}

	return GetBoxPID(BoxIdOne) == GetBoxPID(BoxIdTwo);
}

void AYortGridMesh::WinCondition(int32 BoxIdOne, int32 BoxIdTwo, int32 BoxIdThree)
{
	//Upon winning the game, we're going to destroy all of the box components that aren't on the win condition row and stop the lerp so the boxes stay in place.
	bIsLerping = false;

	for (const TPair<int32, UAimToeBoxComponent*>& pair : GridMap)
	{
		if (pair.Key != BoxIdOne && pair.Key != BoxIdTwo && pair.Key != BoxIdThree)
		{
			pair.Value->DestroyComponent();
		}
	}
}



