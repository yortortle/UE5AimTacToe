// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "AimToeBoxComponent.h"
#include "YortGridMesh.generated.h"

class UBoxComponent;

UCLASS()
class AIMTOE_API AYortGridMesh : public AActor
{
	GENERATED_BODY()

public:
	AYortGridMesh();

	/** Public editor exposed float so designers can set the speed of the moving boxes in the blueprint in the editor themselves*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpSpeed;

protected:
	/** This funciton will generate our box components in the constructor to make our Tic Tac Toe grid*/
	UAimToeBoxComponent* CreateBoxComponent(UStaticMesh* MeshToUse, FVector Offset, FName ComponentName, int32 BoxID, bool bIsRootComponent = false);

	/** A map used to store our tic tac toe grid components with their corresponds assigned ID's to compute the win condition logic later on*/
	TMap<int32, UAimToeBoxComponent*> GridMap;

	/** Used to turn the lerp on or off*/
	bool bIsLerping;

	/** This float stores the value that we plug into the sin function to get the respective percent for our lerp*/
	float LerpAlpha;

	/** called when projectile hits something, is set as a UFUNCTION() macro to be bound to a delegate
	This function handles most of our win condition logic, as the best time to check whether a win condition is active is when another block has been activated.
	*/
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Tick function overriden from parent class AActor, used for lerp */
	virtual void Tick(float DeltaSeconds) override;

	/* Created two blueprintimplementablevent to be called when the tic tac toe meshes are hit by bullets, this allows
	vfx artists or designers to go into the editor and call their specific material swaps or VFX changes */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerOneActivateBlock(UPrimitiveComponent* HitComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerTwoActivateBlock(UPrimitiveComponent* HitComponent);

	/* designed to go into the box map from the passed in Box ID and grab the player ID stored on the box from the hit*/
	int32 GetBoxPID(int32 BoxID);

	/* Designed to run the conditional logic to make the win condition if else block more clean and modular, checks to see if default value and then compares player ID's for boxes and returns the evaluation*/
	bool CompareBoxes(int32 BoxIdOne, int32 BxIdTwo);

	void WinCondition(int32 BoxIdOne, int32 BoxIdTwo, int32 BoxIdThree);
};
