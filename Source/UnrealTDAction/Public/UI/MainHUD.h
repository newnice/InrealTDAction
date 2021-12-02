// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTDACTION_API AMainHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSoftClassPtr<UUserWidget> MainScreenTemplate;

	UPROPERTY(EditDefaultsOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSoftClassPtr<UUserWidget> FinalScreenTemplate;

public:
	void ShowMainScreen() const;
	void ShowFinalScreen() const;
protected:
	virtual void BeginPlay() override;
private:
	void ShowScreenWithTemplate(TSoftClassPtr<UUserWidget>) const;
};
