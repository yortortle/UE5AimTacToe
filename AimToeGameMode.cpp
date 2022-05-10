// Copyright Epic Games, Inc. All Rights Reserved.

#include "AimToeGameMode.h"
#include "AimToeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAimToeGameMode::AAimToeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
