// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "LightningAbilityTask.generated.h"

class AEnemyManager;
/**
 * 
 */

DECLARE_DELEGATE(FLightningAbilityTaskDelegate);

UCLASS()
class UNREALTDACTION_API ULightningAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

	float DelayBetweenAttacks;
	float AttackRadius;
	FVector ActorPosition;
	FVector ForwardVector;

	TArray<TTuple<AActor*, float>> EnemiesToKill;
	TWeakObjectPtr<AEnemyManager> EnemyManager;
	FVector PreviousAttackedEnemyLocation;
	FTimerHandle TimerHandle;
	FLightningAbilityTaskDelegate OnCompletedDelegate;
protected:
	virtual void Activate() override;
public:
	static ULightningAbilityTask* CreateLightningTask(UGameplayAbility* OwningAbility,
	                                                  FName TaskInstanceName,
	                                                  float AbilityRadius,
	                                                  float DelayBetweenAttacks,
	                                                  const FVector& StartPoint,
	                                                  const FVector& ForwardVector);

	FLightningAbilityTaskDelegate& OnCompleted()
	{
		return OnCompletedDelegate;
	}

private:
	void ApplyAttack();
	AEnemyManager* FindEnemyManagerActor() const;
	void SortEnemies(TArray<AActor*>& Enemies);
	void FillEnemiesToKill();

	void CompleteTask();

	UFUNCTION()
	void OnTimeFinish();

	void KillSingleEnemy();

	virtual void OnDestroy(bool bInOwnerFinished) override;
};
