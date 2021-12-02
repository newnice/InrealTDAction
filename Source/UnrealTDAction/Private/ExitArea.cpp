// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitArea.h"

AExitArea::AExitArea()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	PrimaryActorTick.bCanEverTick = false;
}

bool AExitArea::IsCollidedWithPawn() const
{
	return BoxComponent->IsOverlappingActor(GetWorld()->GetFirstPlayerController()->GetPawn());
}
