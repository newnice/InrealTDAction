// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "AbilitySystemComponent.h"
#include "CustomMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"


APlayerPawn::APlayerPawn()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	SetRootComponent(StaticMeshComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomComponent"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 180.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UCustomMovementComponent>("MovementComponent");

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");

	PrimaryActorTick.bCanEverTick = false;
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("MoveForward", IE_Pressed, MovementComponent,
	                           &UCustomMovementComponent::StartAccumulateForce);
	InputComponent->BindAction("MoveForward", IE_Released, MovementComponent,
	                           &UCustomMovementComponent::FinishAccumulateForce);

	InputComponent->BindAxis("Rotate", MovementComponent, &UCustomMovementComponent::ApplyRotation);

	InputComponent->BindAction("ChainLightning", IE_Pressed, this, &APlayerPawn::ApplyChainLightningAbility);
}

void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilities();
}

int APlayerPawn::GetPawnLevel() const
{
	return 1;
}

void APlayerPawn::InitAbilities()
{
	if (!AbilitySystemComponent)
		return;
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilityHandler = AbilitySystemComponent->GiveAbility(
		FGameplayAbilitySpec(GrantedAbility, GetPawnLevel(), INDEX_NONE, this));
}

void APlayerPawn::ApplyChainLightningAbility()
{
	AbilitySystemComponent->TryActivateAbility(AbilityHandler);
}

bool APlayerPawn::FreezeMovement(bool IsEnabled)
{
	return MovementComponent->TryFreezeMovement(IsEnabled);
}

UAbilitySystemComponent* APlayerPawn::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
