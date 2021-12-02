// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "LightningAbilityTask.generated.h"

class AEnemyManager;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLightningAbilityTaskDelegate);

UCLASS()
class UNREALTDACTION_API ULightningAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

	float DelayBetweenAttacks;
	float AttackRadius;
	FVector ActorPosition;


	TArray<AActor*> EnemiesToDamage;
	TWeakObjectPtr<AEnemyManager> EnemyManager;
	float TimeStarted;
	int AttackedEnemiesCount;
	FVector PreviousAttackedEnemyLocation;

	void OnTimeFinish();

protected:
	virtual void Activate() override;
public:
	static ULightningAbilityTask* CreateLightningTask(UGameplayAbility* OwningAbility,
	                                                  FName TaskInstanceName,
	                                                  float AbilityRadius,
	                                                  float DelayBetweenAttacks,
	                                                  const FVector& StartPoint);
	FLightningAbilityTaskDelegate OnCompleted;

private:
	void ApplyAttack();
	AEnemyManager* FindEnemyManagerActor() const;
	TArray<AActor*> FindDamagedEnemies() const;
};
