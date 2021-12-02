// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyManager.h"
#include "ExitArea.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealTDActionGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTDACTION_API AUnrealTDActionGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	TWeakObjectPtr<AExitArea> ExitArea;
	TWeakObjectPtr<AEnemyManager> EnemyManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	TArray<FString> LevelsOrder;

public:
	AUnrealTDActionGameModeBase();
	void DoOnNoEnemiesLeft();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DoOnExitReached(AActor* OverlappedActor, AActor* OtherActor);

	void DoOnLevelChanged();
	void FindAndInitExitArea();
	void FindAndInitEnemyManager();
	FString GetNextLevelName();
	void LoadNewLevel(const FString& Name);
	void ApplyGameOver();
	void DoOnLevelCompleted();
};
