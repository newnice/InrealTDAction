// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCustomMovementComponent::UCustomMovementComponent()
{
	MaxAddedVelocity = 25.f;
	MaxTimeMs = 2000;
	FrictionCoefficient = -1;
	CurrentVelocity = 0;
	SimpleRotationAngle = 2;

	PrimaryComponentTick.bCanEverTick = true;
}

void UCustomMovementComponent::ApplyRotation(float Direction)
{
	if (Direction == 0 || !SceneComponentToMove.IsValid())
		return;
	auto RotationAngel = Direction < 0 ? SimpleRotationAngle : SimpleRotationAngle * -1;

	SceneComponentToMove->AddLocalRotation(FRotator{0, RotationAngel, 0});
}

void UCustomMovementComponent::StartAccumulateForce()
{
	StartTimestampToAccumulateForce = FDateTime::Now().GetTicks();
}

void UCustomMovementComponent::FinishAccumulateForce()
{
	int64 DurationMs = (FDateTime::Now().GetTicks() - StartTimestampToAccumulateForce) / ETimespan::TicksPerMillisecond;
	int FinalDurationMs = static_cast<int>(FMath::Clamp<
		int64>(DurationMs, 0, MaxTimeMs));

	float VelocityToAdd = (MaxAddedVelocity * FinalDurationMs) / MaxTimeMs;
	CurrentVelocity = CurrentVelocity + VelocityToAdd;

	StartTimestampToAccumulateForce = 0;
}


// Called when the game starts
void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SceneComponentToMove = GetOwner()->GetRootComponent();
	auto MeshComponent = Cast<UStaticMeshComponent>(
		GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	FVector Min{};
	MeshComponent->GetLocalBounds(Min, MaxBounds);
}

FVector UCustomMovementComponent::CorrectPathDueToPhysics(float DistanceToPass)
{
	if (DistanceToPass <= 0)
		return FVector::ZeroVector;
	FHitResult OutHitResult;

	const auto TraceStart = SceneComponentToMove->GetComponentLocation() + SceneComponentToMove->GetForwardVector() *
		MaxBounds.X / 2;
	const auto TraceEnd = DistanceToPass * SceneComponentToMove->GetForwardVector() + TraceStart;

	if (GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_WorldStatic))
	{
		if (OutHitResult.Distance - MaxBounds.X / 2 <= 0)
		{
			const auto NewForward = SceneComponentToMove->GetForwardVector() + 2 * OutHitResult.ImpactNormal;
			const FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(NewForward, FVector::UpVector);

			SceneComponentToMove->SetWorldRotation(NewRotation);
			return NewForward * DistanceToPass;
		}
		return OutHitResult.Distance * SceneComponentToMove->GetForwardVector();
	}
	return DistanceToPass * SceneComponentToMove->GetForwardVector();
}

// Called every frame
void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SceneComponentToMove.IsValid())
		return;

	const auto VelocityAfter = CurrentVelocity + FrictionCoefficient * DeltaTime;
	const auto DistanceToPass = CurrentVelocity * DeltaTime + FrictionCoefficient * DeltaTime * DeltaTime / 2;

	SceneComponentToMove->AddWorldOffset(CorrectPathDueToPhysics(DistanceToPass));
	CurrentVelocity = VelocityAfter;
}
