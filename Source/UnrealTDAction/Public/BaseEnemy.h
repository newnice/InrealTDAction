// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.generated.h"

UCLASS()
class UNREALTDACTION_API ABaseEnemy : public AActor
{
	GENERATED_BODY()
	void DestroyEnemy( );
public:
	ABaseEnemy();


protected:
	virtual void BeginPlay() override;
};
