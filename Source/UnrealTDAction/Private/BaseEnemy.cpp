// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"


void ABaseEnemy::DestroyEnemy()
{
	bool isGenerateBridge = FMath::RandBool();
	Destroy();	
}

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}
