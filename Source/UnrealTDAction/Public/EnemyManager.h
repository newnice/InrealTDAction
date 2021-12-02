// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

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
public:
	// Sets default values for this actor's properties
	AEnemyManager();

	bool IsAnyEnemyAlive() const;
	void KillEnemy(AActor* Enemy);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void TryGenerateBridge();
	void GenerateEnemies();
	FVector CalculateEnemyPosition(const FVector& PlatformPosition, const FVector& PlatformBounds) const;
	UFUNCTION()
	void OnBeginEnemyOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
