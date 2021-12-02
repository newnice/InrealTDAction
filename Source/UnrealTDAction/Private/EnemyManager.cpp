// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include <gameux.h>

#include "AUnrealTDActionGameInstance.h"
#include "UnrealTDActionGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GeneratedBridgesAmount = 0;
}

bool AEnemyManager::IsAnyEnemyAlive() const
{
	return GeneratedEnemies.Num() > 0;
}

void AEnemyManager::KillEnemy(AActor* Enemy)
{
	if (!Enemy)
		return;

	Cast<UAUnrealTDActionGameInstance>(GetWorld()->GetGameInstance())->IncreaseScore();
	GeneratedEnemies.RemoveSingle(Enemy);
	Enemy->OnActorBeginOverlap.RemoveAll(this);
	Enemy->Destroy();
	TryGenerateBridge();
	
	if (!IsAnyEnemyAlive())
	{
		Cast<AUnrealTDActionGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->DoOnNoEnemiesLeft();
	}
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	GenerateEnemies();
}

void AEnemyManager::TryGenerateBridge()
{
	int LeftBridges = PlacesForBridges.Num() - GeneratedBridgesAmount;
	if (LeftBridges <= 0)
		return;

	//probability changes to allow all bridges be generated
	auto ProbabilityToGenerate = LeftBridges * 1.f / GeneratedEnemies.Num();
	if (ProbabilityToGenerate >= FMath::RandRange(0.f, 1.f))
	{
		auto Place = PlacesForBridges[GeneratedBridgesAmount];

		AActor* Bridge = GetWorld()->SpawnActor(BridgeTemplate);
		Bridge->AttachToActor(Place, FAttachmentTransformRules::SnapToTargetIncludingScale);

		GeneratedBridgesAmount++;
	}
}

FVector AEnemyManager::CalculateEnemyPosition(const FVector& PlatformPosition, const FVector& PlatformBounds) const
{
	auto Position = FVector{
		FMath::RandRange(0.f, 1.f) * PlatformBounds.X, FMath::RandRange(0.f, 1.f) * PlatformBounds.Y, PlatformBounds.Z
	};

	return Position + PlatformPosition;
}

void AEnemyManager::GenerateEnemies()
{
	for (auto& Platform : PlatformsToGenerate)
	{
		if (!Platform.Key)
		{
			continue;
		}
		auto Mesh = Cast<UStaticMeshComponent>(Platform.Key->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (!Mesh)
			continue;

		FVector MinBounds, MaxBounds;
		Mesh->GetLocalBounds(MinBounds, MaxBounds);

		for (auto i = 0; i < Platform.Value; i++)
		{
			AActor* Enemy = GetWorld()->SpawnActor(EnemyType);
			Enemy->OnActorBeginOverlap.AddDynamic(this, &AEnemyManager::OnBeginEnemyOverlap);
			Enemy->SetActorLocation(CalculateEnemyPosition(Platform.Key->GetActorLocation(), MaxBounds));
			Enemy->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			GeneratedEnemies.Add(Enemy);
		}
	}
}

void AEnemyManager::OnBeginEnemyOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	KillEnemy(OverlappedActor);
}
