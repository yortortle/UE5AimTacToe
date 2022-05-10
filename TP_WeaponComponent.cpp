// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "AimToeCharacter.h"
#include "AimToeProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if(Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	if (Ammo < 1)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			AAimToeProjectile* proj = World->SpawnActor<AAimToeProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			// We take the character who shot the bullet and grab the assigned ID off of them, which we then transfer to the projectile and swap the character id
			// this is a very hacky way to do things, and if reworked later would have two seperate pawns which would switch possession on turn switch
			// given the time constraints of the project I decided to flip a "Character ID" from 0 to 1 to signify the changing of turns.
			if (Character->CharacterID == 0)
			{
				proj->ProjectileID = 1;
				Character->CharacterID = 1;
			}
			else if (Character->CharacterID == 1)
			{
				proj->ProjectileID = 0;
				Character->CharacterID = 0;
			}
		}
		
		// I use this timer to reset the ammo to 1 which signifies the next players turn has begun.
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &UTP_WeaponComponent::GunFireCooldownOver, 0.5f, false);
		//
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	Ammo -= 1;
}

void UTP_WeaponComponent::GunFireCooldownOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Switching ID's"));
	Ammo = 1;
	OnComponentCooldownChanged.Broadcast(Character->CharacterID);
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(Character != nullptr)
	{
		// Unregister from the OnUseItem Event
		Character->OnUseItem.RemoveDynamic(this, &UTP_WeaponComponent::Fire);
	}
}

void UTP_WeaponComponent::AttachWeapon(AAimToeCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if(Character != nullptr)
	{
		// Attach the weapon to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		GetOwner()->AttachToComponent(Character->GetMesh1P(),AttachmentRules, FName(TEXT("GripPoint")));

		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnUseItem.AddDynamic(this, &UTP_WeaponComponent::Fire);
	}
}

