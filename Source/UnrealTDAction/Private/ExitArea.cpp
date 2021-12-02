// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitArea.h"

AExitArea::AExitArea()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PrimaryActorTick.bCanEverTick = false;
}