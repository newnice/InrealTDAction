// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"
#include "Blueprint/UserWidget.h"


void AMainHUD::ShowMainScreen() const
{
	ShowScreenWithTemplate(MainScreenTemplate);
}

void AMainHUD::ShowFinalScreen() const
{
	ShowScreenWithTemplate(FinalScreenTemplate);
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	ShowMainScreen();
}

void AMainHUD::ShowScreenWithTemplate(TSoftClassPtr<UUserWidget> Template) const
{
	auto ScreenClass = Template.LoadSynchronous();
	const auto Screen = CreateWidget<UUserWidget>(GetWorld(), ScreenClass);
	if (Screen)
		Screen->AddToViewport();
}
