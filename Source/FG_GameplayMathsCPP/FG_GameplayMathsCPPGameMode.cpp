// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_GameplayMathsCPPGameMode.h"
#include "FG_GameplayMathsCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFG_GameplayMathsCPPGameMode::AFG_GameplayMathsCPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
