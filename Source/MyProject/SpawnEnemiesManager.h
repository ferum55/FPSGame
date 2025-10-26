// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "SpawnEnemiesManager.generated.h"

UCLASS()
class MYPROJECT_API ASpawnEnemiesManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemy> BaseEnemyBP;
	// Sets default values for this actor's properties
	ASpawnEnemiesManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
