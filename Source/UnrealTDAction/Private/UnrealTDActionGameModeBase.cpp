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
		EnemyManager->OnNoEnemiesLeft().AddUObject(this, &AUnrealTDActionGameModeBase::DoOnNoEnemiesLeft);
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
}

void AUnrealTDActionGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	DoOnLevelChanged();
}

void AUnrealTDActionGameModeBase::ApplyGameOver()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	auto HUD = Cast<AMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	HUD->ShowFinalScreen();
}

void AUnrealTDActionGameModeBase::DoOnLevelCompleted()
{
	auto NextLevel = GetNextLevelName();
	if (NextLevel.IsEmpty())
	{
		ApplyGameOver();
	}
	else
	{
		LoadNewLevel(NextLevel);
	}
}

void AUnrealTDActionGameModeBase::DoOnExitReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != GetWorld()->GetFirstPlayerController()->GetPawn())
		return;
	if (EnemyManager.IsValid() && EnemyManager->IsAnyEnemyAlive())
	{
		return;
	}

	DoOnLevelCompleted();
}

void AUnrealTDActionGameModeBase::DoOnNoEnemiesLeft()
{
	if (ExitArea.IsValid() && ExitArea->IsCollidedWithPawn())
	{
		DoOnLevelCompleted();
	}
}

void AUnrealTDActionGameModeBase::ClearData()
{
	if (ExitArea.IsValid())
		ExitArea->OnActorBeginOverlap.RemoveAll(this);
	if (EnemyManager.IsValid())
		EnemyManager->OnNoEnemiesLeft().RemoveAll(this);
}

void AUnrealTDActionGameModeBase::LoadNewLevel(const FString& Name)
{
	ClearData();

	UGameplayStatics::OpenLevel(GetWorld(), FName(*Name));
	DoOnLevelChanged();
}
