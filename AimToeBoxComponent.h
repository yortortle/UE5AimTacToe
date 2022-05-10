// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "AimToeBoxComponent.generated.h"

UCLASS()
class AIMTOE_API UAimToeBoxComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UAimToeBoxComponent();

	//Used to signify place in the grid for tik tak toe
	UPROPERTY(BlueprintReadOnly)
	int32 BoxID = -1;

	//value set to the player ID which activates this box component
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerID = -1;
};
