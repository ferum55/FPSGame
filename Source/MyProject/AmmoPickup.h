// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AAmmoPickup : public APickupBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Pickup")
	int32 AmmoAmount = 15;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
