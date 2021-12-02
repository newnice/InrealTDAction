// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMovementComponent.h"

#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCustomMovementComponent::UCustomMovementComponent()
{
	MaxAddedVelocity = 400.f;
	MaxTimeMs = 2000;
	FrictionCoefficient = -10;
	CurrentVelocity = 0;
	SimpleRotationAngle = 2;

	PrimaryComponentTick.bCanEverTick = true;
}

void UCustomMovementComponent::ApplyRotation(float Direction)
{
	if (Direction == 0 || !SceneComponentToMove.IsValid())
		return;
	const auto RotationAngel = Direction < 0 ? SimpleRotationAngle : SimpleRotationAngle * -1;

	SceneComponentToMove->AddLocalRotation(FRotator{0, RotationAngel, 0});
}

void UCustomMovementComponent::StartAccumulateForce()
{
	StartTimestampToAccumulateForce = FDateTime::Now().GetTicks();
}

void UCustomMovementComponent::FinishAccumulateForce()
{
	const int64 DurationMs = (FDateTime::Now().GetTicks() - StartTimestampToAccumulateForce) /
		ETimespan::TicksPerMillisecond;
	const int FinalDurationMs = static_cast<int>(FMath::Clamp<
		int64>(DurationMs, 0, MaxTimeMs));

	const float VelocityToAdd = (MaxAddedVelocity * FinalDurationMs) / MaxTimeMs;
	CurrentVelocity = CurrentVelocity + VelocityToAdd;

	StartTimestampToAccumulateForce = 0;
}


void UCustomMovementComponent::CalculateMaxBounds()
{
	auto MeshComponent = Cast<UStaticMeshComponent>(
		GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	FVector Min{};
	MeshComponent->GetLocalBounds(Min, MaxBounds);
}

// Called when the game starts
void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SceneComponentToMove = GetOwner()->GetRootComponent();
	CalculateMaxBounds();
}


void UCustomMovementComponent::TeleportToStart()
{
	auto StartSpot = GetWorld()->GetFirstPlayerController()->StartSpot;
	if (StartSpot.IsValid())
	{
		FRotator SpawnRotation(ForceInit);
		SpawnRotation.Yaw = StartSpot->GetActorRotation().Yaw;
		SceneComponentToMove->SetWorldLocationAndRotation
			(StartSpot->GetActorLocation(), SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	CurrentVelocity = 0;
	CurrentFlyVelocity = 0;
}

void UCustomMovementComponent::CorrectFlyVelocityByGround(float DeltaTime)
{
	auto FlyVelocity = CurrentFlyVelocity + 980 * DeltaTime;

	FHitResult OutHitGroundResult;
	auto TraceStart = SceneComponentToMove->GetComponentLocation();
	auto TraceEnd = (MaxBounds.Z + FlyVelocity * DeltaTime) * FVector::DownVector + TraceStart;

	bool IsExistGround = GetWorld()->
		LineTraceSingleByChannel(OutHitGroundResult, TraceStart, TraceEnd, ECC_WorldStatic);
	CurrentFlyVelocity = IsExistGround ? 0 : FlyVelocity;
}

FVector UCustomMovementComponent::CalculateLocalOffset(float DeltaTime) const
{
	auto DistanceToPass = CurrentVelocity * SceneComponentToMove->GetForwardVector() * DeltaTime + CurrentFlyVelocity *
		FVector::DownVector * DeltaTime;

	FHitResult OutHitResult;
	auto TraceStart = SceneComponentToMove->GetComponentLocation() + SceneComponentToMove->GetForwardVector() *
		MaxBounds.X / 2;
	auto TraceEnd = DistanceToPass + TraceStart;

	if (GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_WorldStatic))
	{
		if (OutHitResult.Distance - MaxBounds.X / 2 <= 0)
		{
			const auto NewForward = SceneComponentToMove->GetForwardVector() + 2 * OutHitResult.ImpactNormal;
			const FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(NewForward, FVector::UpVector);

			SceneComponentToMove->SetWorldRotation(NewRotation);
			return OutHitResult.Distance * NewForward;
		}

		return OutHitResult.Distance * SceneComponentToMove->GetForwardVector();
	}
	return DistanceToPass;
}

void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SceneComponentToMove.IsValid())
		return;


	if (SceneComponentToMove->GetComponentLocation().Z <= GetWorld()->GetWorldSettings()->KillZ)
	{
		TeleportToStart();
		return;
	}

	CorrectFlyVelocityByGround(DeltaTime);
	CurrentVelocity = FMath::Max(0.f, CurrentVelocity + FrictionCoefficient * DeltaTime);
	SceneComponentToMove->AddWorldOffset(CalculateLocalOffset(DeltaTime));
}
