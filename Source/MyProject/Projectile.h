// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Projectile.generated.h"


UCLASS()
class MYPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION() 
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnProjectileStop(const FHitResult& Hit);

	void SpawnImpactEffect(const FHitResult& Hit);

	UPROPERTY(VisibleDefaultsOnly,Category=Projectile)
	USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere,Category=Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;
	UPROPERTY(EditAnywhere,Category=Projectile)
	float LifeSpan = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UParticleSystem* HitEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* HitNiagaraEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterialInterface* HitDecal;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FireInDirection(const FVector& Direction);
};
