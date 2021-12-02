// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetMainScreen.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTDACTION_API UWidgetMainScreen : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UTextBlock* TxtScore;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	UFUNCTION()
	void UpdateScore(int NewScore);
};
