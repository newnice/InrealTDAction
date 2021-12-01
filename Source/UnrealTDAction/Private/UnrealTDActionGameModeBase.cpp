// Copyright Epic Games, Inc. All Rights Reserved.


#include "UnrealTDActionGameModeBase.h"

AUnrealTDActionGameModeBase::AUnrealTDActionGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_PlayerPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}	
}
