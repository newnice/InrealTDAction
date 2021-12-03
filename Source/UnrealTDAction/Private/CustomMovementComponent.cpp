// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMovementComponent.h"

#include "EngineUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

UCustomMovementComponent::UCustomMovementComponent()
{
	MinAddedVelocity = 100.f;
	MaxAddedVelocity = 400.f;
	MaxTimeMs = 2000;
	FrictionCoefficient = -10;
	CurrentVelocity = 0;
	SimpleRotationAngle = 2;
	IsFreeze = false;
	FreeFlyAcceleration = 980;
	PrimaryComponentTick.bCanEverTick = true;
}

void UCustomMovementComponent::ApplyRotation(float Direction)
{
	if (IsFreeze || Direction == 0 || !SceneComponentToMove.IsValid() || !FMath::IsNearlyZero(CurrentFlyVelocity))
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
	if (!IsFreeze && FMath::IsNearlyZero(CurrentFlyVelocity))
	{
		const int64 DurationMs = (FDateTime::Now().GetTicks() - StartTimestampToAccumulateForce) /
			ETimespan::TicksPerMillisecond;
		const int FinalDurationMs = static_cast<int>(FMath::Clamp<
			int64>(DurationMs, 0, MaxTimeMs));

		const float VelocityToAdd = ((MaxAddedVelocity - MinAddedVelocity) * FinalDurationMs) / MaxTimeMs +
			MinAddedVelocity;
		CurrentVelocity = CurrentVelocity + VelocityToAdd;
	}

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

FVector UCustomMovementComponent::CalculateFlyDistance(float DeltaTime)
{
	auto DistanceToPassDown = CalculateDistanceToPass(CurrentFlyVelocity, FreeFlyAcceleration, DeltaTime);

	FHitResult OutHitGround;
	auto TraceStart = SceneComponentToMove->GetComponentLocation();
	auto TraceEnd = (MaxBounds.Z + DistanceToPassDown) * FVector::DownVector + TraceStart;

	bool IsExistGround = GetWorld()->LineTraceSingleByChannel(OutHitGround, TraceStart, TraceEnd, ECC_WorldStatic);

	if (IsExistGround)
	{
		CurrentFlyVelocity = 0;
		return FVector::ZeroVector;
	}

	CurrentFlyVelocity = CalculateNextVelocity(CurrentFlyVelocity, FreeFlyAcceleration, DeltaTime);
	return DistanceToPassDown * FVector::DownVector;
}

FVector UCustomMovementComponent::CorrectDistanceByWalls(float DeltaTime) const
{
	auto DistanceToPass = CalculateDistanceToPass(CurrentVelocity, FrictionCoefficient, DeltaTime) *
		SceneComponentToMove->GetForwardVector();

	FHitResult OutHitResult;
	auto TraceStart = SceneComponentToMove->GetComponentLocation();
	auto TraceEnd = DistanceToPass + TraceStart + SceneComponentToMove->GetForwardVector() *
		MaxBounds.X / 2;

	auto HitExist = GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_WorldStatic);


	if (HitExist)
	{
		auto DistanceToWall = OutHitResult.Distance - MaxBounds.X / 2;
		if (FMath::IsNearlyZero(DistanceToWall, 0.1f) || DistanceToWall < 0)
		{
			const auto NewForward = SceneComponentToMove->GetForwardVector() + 2 * OutHitResult.ImpactNormal;
			const FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(NewForward, FVector::UpVector);
			SceneComponentToMove->SetWorldRotation(NewRotation);

			return CalculateDistanceToPass(CurrentVelocity, FrictionCoefficient, DeltaTime) * NewForward;
		}

		return DistanceToWall * SceneComponentToMove->GetForwardVector();
	}

	return DistanceToPass;
}

FVector UCustomMovementComponent::CalculateGroundDistance(float DeltaTime)
{
	FVector GroundDistance;
	if (FMath::IsNearlyZero(CurrentFlyVelocity))
		GroundDistance = CorrectDistanceByWalls(DeltaTime);
	else
		GroundDistance = CalculateDistanceToPass(CurrentVelocity, FrictionCoefficient, DeltaTime) * SceneComponentToMove
			->GetForwardVector();

	CurrentVelocity = CalculateNextVelocity(CurrentVelocity, FrictionCoefficient, DeltaTime);

	return GroundDistance;
}

float UCustomMovementComponent::CalculateDistanceToPass(float Velocity, float Acceleration, float DeltaTime) const
{
	return Velocity * DeltaTime + Acceleration * DeltaTime * DeltaTime / 2;
}

float UCustomMovementComponent::CalculateNextVelocity(float Velocity, float Acceleration, float DeltaTime) const
{
	return FMath::Max(0.f, Velocity + Acceleration * DeltaTime);
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

	auto DistanceByFly = CalculateFlyDistance(DeltaTime);
	auto DistanceByGround = CalculateGroundDistance(DeltaTime);
	SceneComponentToMove->AddWorldOffset(DistanceByFly + DistanceByGround);
}

bool UCustomMovementComponent::TryFreezeMovement(bool IsFreezeEnabled)
{
	if (!FMath::IsNearlyZero(CurrentFlyVelocity))
	{
		IsFreeze = false;
		return false;
	}

	IsFreeze = IsFreezeEnabled;
	if (IsFreeze)
	{
		CurrentVelocity = 0;
		CurrentFlyVelocity = 0;
	}
	return true;
}
