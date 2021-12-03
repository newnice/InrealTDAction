// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LightningAbilityTask.h"

#include "BaseEnemy.h"
#include "DrawDebugHelpers.h"
#include "EnemyManager.h"
#include "EngineUtils.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Kismet/KismetSystemLibrary.h"


ULightningAbilityTask* ULightningAbilityTask::CreateLightningTask(UGameplayAbility* OwningAbility,
                                                                  FName TaskInstanceName,
                                                                  float AttackRadius,
                                                                  float DelayBetweenAttacks,
                                                                  const FVector& StartPoint,
                                                                  const FVector& ForwardVector)
{
	ULightningAbilityTask* LightningTask = NewAbilityTask<ULightningAbilityTask>(OwningAbility, TaskInstanceName);
	LightningTask->DelayBetweenAttacks = DelayBetweenAttacks;
	LightningTask->AttackRadius = AttackRadius;
	LightningTask->ActorPosition = StartPoint;
	LightningTask->ForwardVector = ForwardVector;
	LightningTask->PreviousAttackedEnemyLocation = StartPoint;
	return LightningTask;
}

void ULightningAbilityTask::KillSingleEnemy()
{
	auto EnemyIndex = EnemiesToDamage.Num() - 1;
	const auto EnemyToKill = EnemiesToDamage[EnemyIndex];
	DrawDebugLine(GetWorld(), PreviousAttackedEnemyLocation, EnemyToKill->GetActorLocation(), FColor::Red, false,
	              DelayBetweenAttacks + 0.5, 0, 3.f);

	PreviousAttackedEnemyLocation = EnemyToKill->GetActorLocation();
	EnemiesToDamage.RemoveAt(EnemyIndex);
	EnemyManager->KillEnemy(EnemyToKill);
}

void ULightningAbilityTask::ApplyAttack()
{
	KillSingleEnemy();
	if (EnemiesToDamage.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULightningAbilityTask::OnTimeFinish,
		                                       DelayBetweenAttacks,
		                                       true);
	}
	else
	{
		EndTask();
	}
}


void ULightningAbilityTask::OnTimeFinish()
{
	KillSingleEnemy();

	if(EnemiesToDamage.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		OnCompleted.Broadcast();
		EndTask();
	}
}

TArray<AActor*> ULightningAbilityTask::FindDamagedEnemies() const
{
	TArray<AActor*> OverlapedEnemies;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ActorPosition, AttackRadius, TraceObjectTypes,
	                                          ABaseEnemy::StaticClass(), TArray<AActor*>{}, OverlapedEnemies);

	return SortEnemies(OverlapedEnemies);
}

void ULightningAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	EnemiesToDamage.Empty();
	OnCompleted.Clear();
	Super::OnDestroy(bInOwnerFinished);
}

AEnemyManager* ULightningAbilityTask::FindEnemyManagerActor() const
{
	TActorIterator<AEnemyManager> ActorItr(GetWorld());
	if (ActorItr)
	{
		return *ActorItr;
	}
	return nullptr;
}

TArray<AActor*> ULightningAbilityTask::SortEnemies(TArray<AActor*>& EnemiesToKill) const
{
	TMap<AActor*, float> MapForSort{};

	for (auto& Enemy : EnemiesToKill)
	{
		auto ActorVector = Enemy->GetActorLocation() - this->ActorPosition;
		ActorVector.Normalize();
		auto Dot = FVector::DotProduct(ActorVector, ForwardVector);
		auto Cross = ActorVector.X * ForwardVector.Y - ActorVector.Y * ForwardVector.X;
		auto SortPoints = Cross < 0 ? Dot : -(2 + Dot);
		MapForSort.Add(Enemy, SortPoints);
	}

	MapForSort.ValueSort([](float A, float B)
	{
		return A <= B;
	});


	TArray<AActor*> Result;
	MapForSort.GenerateKeyArray(Result);

	return Result;
}

void ULightningAbilityTask::Activate()
{
	Super::Activate();
	DrawCircle(GetWorld(), ActorPosition, FVector::ForwardVector, FVector::RightVector, FColor::Blue, AttackRadius,
	           32,
	           false, 1, 0, 3);

	EnemyManager = FindEnemyManagerActor();
	if (!EnemyManager.IsValid())
	{
		OnCompleted.Broadcast();
		EndTask();
		return;
	}
	EnemiesToDamage = FindDamagedEnemies();
	if (EnemiesToDamage.Num() == 0)
	{
		OnCompleted.Broadcast();
		EndTask();
		return;
	}

	DrawCircle(GetWorld(), ActorPosition, FVector::ForwardVector, FVector::RightVector, FColor::Blue, AttackRadius, 32,
	           false,
	           DelayBetweenAttacks * EnemiesToDamage.Num() + 1, 0, 3);

	ApplyAttack();
}
