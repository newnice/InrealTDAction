// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LightningAbilityTask.h"

#include "BaseEnemy.h"
#include "DrawDebugHelpers.h"
#include "EnemyManager.h"
#include "EngineUtils.h"
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
	auto EnemyIndex = EnemiesToKill.Num() - 1;
	const auto EnemyToKill = EnemiesToKill[EnemyIndex].Key;
	DrawDebugLine(GetWorld(), PreviousAttackedEnemyLocation, EnemyToKill->GetActorLocation(), FColor::Red, false,
	              DelayBetweenAttacks + 0.5, 0, 3.f);

	PreviousAttackedEnemyLocation = EnemyToKill->GetActorLocation();
	EnemiesToKill.RemoveAt(EnemyIndex);
	EnemyManager->KillEnemy(EnemyToKill);
}

void ULightningAbilityTask::ApplyAttack()
{
	KillSingleEnemy();
	if (EnemiesToKill.Num() > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULightningAbilityTask::OnTimeFinish,
		                                       DelayBetweenAttacks,
		                                       true);
	}
	else
	{
		CompleteTask();
	}
}


void ULightningAbilityTask::OnTimeFinish()
{
	KillSingleEnemy();

	if (EnemiesToKill.Num() == 0)
	{
		CompleteTask();
	}
}

void ULightningAbilityTask::FillEnemiesToKill()
{
	TArray<AActor*> OverlapedEnemies;
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), ActorPosition, AttackRadius, TraceObjectTypes,
	                                          ABaseEnemy::StaticClass(), TArray<AActor*>{}, OverlapedEnemies);

	SortEnemies(OverlapedEnemies);
}

void ULightningAbilityTask::CompleteTask()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	OnCompletedDelegate.ExecuteIfBound();
	EndTask();
}

void ULightningAbilityTask::OnDestroy(bool bInOwnerFinished)
{
	EnemiesToKill.Empty();
	OnCompletedDelegate.Unbind();
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

void ULightningAbilityTask::SortEnemies(TArray<AActor*>& UnsortedEnemies)
{
	for (auto& Enemy : UnsortedEnemies)
	{
		auto ActorVector = Enemy->GetActorLocation() - this->ActorPosition;
		ActorVector.Normalize();
		auto Dot = FVector::DotProduct(ActorVector, ForwardVector);
		auto CrossZ = ForwardVector.X * ActorVector.Y - ActorVector.X * ForwardVector.Y;
		auto SortPoints = CrossZ > 0 ? Dot : -(2 + Dot);
		EnemiesToKill.Emplace(Enemy, SortPoints);
	}

	EnemiesToKill.Sort([](TPair<AActor*, float> A, TPair<AActor*, float> B)
	{
		return A.Value <= B.Value;
	});
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
		CompleteTask();
		return;
	}
	FillEnemiesToKill();
	if (EnemiesToKill.Num() == 0)
	{
		CompleteTask();
		return;
	}

	DrawCircle(GetWorld(), ActorPosition, FVector::ForwardVector, FVector::RightVector, FColor::Blue, AttackRadius, 32,
	           false,
	           DelayBetweenAttacks * EnemiesToKill.Num() + 1, 0, 3);

	ApplyAttack();
}
