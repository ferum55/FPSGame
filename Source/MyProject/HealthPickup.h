// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AHealthPickup : public APickupBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float HealAmount = -30.f;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
