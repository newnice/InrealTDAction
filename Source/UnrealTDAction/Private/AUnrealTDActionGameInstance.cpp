// Fill out your copyright notice in the Description page of Project Settings.


#include "AUnrealTDActionGameInstance.h"

void UAUnrealTDActionGameInstance::IncreaseScore()
{
	CurrentScore++;
	if (OnScoreChangedDelegate.IsBound())
		OnScoreChangedDelegate.Broadcast(CurrentScore);
}