// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnEnemiesManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SpawnEnemyTargetPoint.h"

// Sets default values
ASpawnEnemiesManager::ASpawnEnemiesManager()
{

	PrimaryActorTick.bCanEverTick = true;

}


void ASpawnEnemiesManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnEnemyTargetPoint::StaticClass(), FoundTargetPoints);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (AActor* FoundTargetPoint : FoundTargetPoints) {
		GetWorld()->SpawnActor<AEnemy>(BaseEnemyBP, FoundTargetPoint->GetActorTransform(), SpawnParams);
	}
}

// Called every frame
void ASpawnEnemiesManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

