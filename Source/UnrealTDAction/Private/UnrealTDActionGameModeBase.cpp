// Copyright Epic Games, Inc. All Rights Reserved.


#include "UnrealTDActionGameModeBase.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainHUD.h"

AUnrealTDActionGameModeBase::AUnrealTDActionGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/BP_PlayerPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/UI/BP_MainHUD"));
	if (HUDBPClass.Class != nullptr)
	{
		HUDClass = HUDBPClass.Class;
	}
}

void AUnrealTDActionGameModeBase::FindAndInitExitArea()
{
	TActorIterator<AExitArea> ActorItr(GetWorld());
	if (ActorItr)
	{
		ExitArea = *ActorItr;
		ExitArea->OnActorBeginOverlap.AddDynamic(this, &AUnrealTDActionGameModeBase::DoOnExitReached);
	}
}

void AUnrealTDActionGameModeBase::FindAndInitEnemyManager()
{
	TActorIterator<AEnemyManager> ActorItr(GetWorld());
	if (ActorItr)
	{
		EnemyManager = *ActorItr;
	}
}

FString AUnrealTDActionGameModeBase::GetNextLevelName()
{
	const auto LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	for (auto i = 0; i < LevelsOrder.Num(); i++)
	{
		if (LevelsOrder[i] == LevelName && i < LevelsOrder.Num() - 1)
		{
			return LevelsOrder[i + 1];
		}
	}
	return FString{};
}

void AUnrealTDActionGameModeBase::DoOnLevelChanged()
{
	FindAndInitExitArea();
	FindAndInitEnemyManager();

	if (!EnemyManager.IsValid() || !ExitArea.IsValid())
		return;

	ExitArea->SetActorEnableCollision(!EnemyManager->IsAnyEnemyAlive());
}

void AUnrealTDActionGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->OnLevelsChanged().AddUObject(this, &AUnrealTDActionGameModeBase::DoOnLevelChanged);
}

void AUnrealTDActionGameModeBase::DoGameOver()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	auto HUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	HUD->ShowFinalScreen();
}

void AUnrealTDActionGameModeBase::DoOnExitReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != GetWorld()->GetFirstPlayerController()->GetPawn())
		return;

	auto NextLevel = GetNextLevelName();
	if (NextLevel.IsEmpty())
	{
		DoGameOver();
	}
	else
	{
		LoadNewLevel(NextLevel);
	}
}

void AUnrealTDActionGameModeBase::DoOnNoEnemiesLeft()
{
	ExitArea->SetActorEnableCollision(true);
}

void AUnrealTDActionGameModeBase::LoadNewLevel(const FString& Name)
{
	if (!GetWorld()->GetCurrentLevel()->IsPersistentLevel())
	{
		auto LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName{*LevelName}, FLatentActionInfo{}, true);
	}

	UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*Name), true, true, FLatentActionInfo{});
}
