// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetMainScreen.h"

#include "AUnrealTDActionGameInstance.h"
#include "Components/TextBlock.h"

void UWidgetMainScreen::NativeConstruct()
{
	Super::NativeConstruct();
	auto GameInstance = Cast<UAUnrealTDActionGameInstance>(GetWorld()->GetGameInstance());
	GameInstance->OnScoreChanged().AddDynamic(this, &UWidgetMainScreen::UpdateScore);
}

void UWidgetMainScreen::NativeDestruct()
{
	auto GameInstance = Cast<UAUnrealTDActionGameInstance>(GetWorld()->GetGameInstance());
	GameInstance->OnScoreChanged().RemoveAll(this);
	Super::NativeDestruct();
}


void UWidgetMainScreen::UpdateScore(int NewScore)
{
	TxtScore->SetText(FText::AsNumber(NewScore));
}
