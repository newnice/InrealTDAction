// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AUnrealTDActionGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int);

UCLASS()
class UNREALTDACTION_API UAUnrealTDActionGameInstance : public UGameInstance
{
	GENERATED_BODY()

	int CurrentScore = 0;
	FOnScoreChanged OnScoreChangedDelegate;
public:
	void IncreaseScore();

	FOnScoreChanged& OnScoreChanged()
	{
		return OnScoreChangedDelegate;
	}

	int GetScore() const { return CurrentScore; }
};
