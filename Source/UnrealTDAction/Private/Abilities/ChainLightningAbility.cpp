// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/ChainLightningAbility.h"

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

	AbilityTask = ULightningAbilityTask::CreateLightningTask(
		this, NAME_None, AbilityRadius, DelayBetweenAttacks, Avatar->GetActorLocation(),
		Avatar->GetActorForwardVector());
	AbilityTask->OnCompleted().BindUObject(this, &UChainLightningAbility::OnTaskCompleted);

	AbilityTask->ReadyForActivation();
}

void UChainLightningAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AbilityTask && AbilityTask->IsActive())
	{
		AbilityTask->ExternalCancel();
	}
	auto Avatar = Cast<APlayerPawn>(CurrentActorInfo->AvatarActor.Get());
	if (Avatar)
	{		
		Avatar->FreezeMovement(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UChainLightningAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateCancelAbility)
{	
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UChainLightningAbility::OnTaskCompleted()
{	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
