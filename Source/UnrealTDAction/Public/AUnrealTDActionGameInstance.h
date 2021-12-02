// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AUnrealTDActionGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int, Score);

UCLASS()
class UNREALTDACTION_API UAUnrealTDActionGameInstance : public UGameInstance
{
	GENERATED_BODY()

	int CurrentScore = 0;
	FOnScoreChanged OnScoreChangedAction;
public:
	void IncreaseScore();

	FOnScoreChanged& OnScoreChanged()
	{
		return OnScoreChangedAction;
	}

	int GetScore() const { return CurrentScore; }
};
