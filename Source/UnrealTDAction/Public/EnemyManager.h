// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnNoEnemiesLeft);

UCLASS()
class UNREALTDACTION_API AEnemyManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	TMap<AActor*, int> PlatformsToGenerate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> EnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PlacesForBridges;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> BridgeTemplate;

	UPROPERTY()
	TArray<AActor*> GeneratedEnemies;

	int GeneratedBridgesAmount;
	FOnNoEnemiesLeft OnNoEnemiesLeftDelegate;
public:
	AEnemyManager();

	bool IsAnyEnemyAlive() const;
	void KillEnemy(AActor* Enemy);

	FOnNoEnemiesLeft& OnNoEnemiesLeft()
	{
		return OnNoEnemiesLeftDelegate;
	}

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginEnemyOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void TryGenerateBridge();
	void GenerateEnemies();
	FVector CalculateEnemyPosition(const FVector& PlatformPosition, const FVector& PlatformBounds) const;
};
