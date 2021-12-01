// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREALTDACTION_API UCustomMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	float MaxAddedVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	int MaxTimeMs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	float FrictionCoefficient;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, meta = (AllowPrivateAccess = "true"))
	float SimpleRotationAngle;

	float CurrentVelocity;
	int64 StartTimestampToAccumulateForce;
	TWeakObjectPtr<USceneComponent> SceneComponentToMove;
	FVector MaxBounds;

public:
	// Sets default values for this component's properties
	UCustomMovementComponent();

	void ApplyRotation(float Direction);
	void StartAccumulateForce();
	void FinishAccumulateForce();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FVector CorrectPathDueToPhysics(float DistanceToPass);
};
