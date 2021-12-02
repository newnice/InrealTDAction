// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/ChainLightningAbility.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "PlayerPawn.h"
#include "Abilities/LightningAbilityTask.h"

UChainLightningAbility::UChainLightningAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityRadius = 10.f;
	DelayBetweenAttacks = 0.1f;
}

void UChainLightningAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	auto Avatar = Cast<APlayerPawn>(ActorInfo->AvatarActor);
	if (!Avatar)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	if (!Avatar->FreezeMovement(true))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	ULightningAbilityTask* Task = ULightningAbilityTask::CreateLightningTask(
		this, NAME_None, AbilityRadius, DelayBetweenAttacks, Avatar->GetActorLocation());
	Task->OnCompleted.AddDynamic(this, &UChainLightningAbility::OnTaskCompleted);

	Task->ReadyForActivation();
}


void UChainLightningAbility::OnTaskCompleted()
{
	auto Avatar = Cast<APlayerPawn>(CurrentActorInfo->AvatarActor.Get());
	if (Avatar)
		Avatar->FreezeMovement(false);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
