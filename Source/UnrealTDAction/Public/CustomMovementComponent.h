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
	float CurrentFlyVelocity;
	int64 StartTimestampToAccumulateForce;
	FVector MaxBounds;

	TWeakObjectPtr<USceneComponent> SceneComponentToMove;
	bool IsFreeze;

public:
	UCustomMovementComponent();

	void ApplyRotation(float Direction);
	void StartAccumulateForce();
	void FinishAccumulateForce();
	void CalculateMaxBounds();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	bool TryFreezeMovement(bool IsEnabled);
protected:
	virtual void BeginPlay() override;
private:
	void TeleportToStart();
	void CorrectFlyVelocityByGround(float DeltaTime);
	FVector CalculateLocalOffset(float DeltaTime) const;
};
