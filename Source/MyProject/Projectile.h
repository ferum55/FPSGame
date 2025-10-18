// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class MYPROJECT_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
   
    AProjectile();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnProjectileStop(const FHitResult& Hit);

    void SpawnImpactEffect(const FHitResult& Hit);

    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = Movement)
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(VisibleAnywhere, Category = Projectile)
    UStaticMeshComponent* ProjectileMeshComponent;

    UPROPERTY(EditAnywhere, Category = Projectile)
    float LifeSpan = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* HitEffect;


    UPROPERTY(EditAnywhere, Category = "Effects")
    UMaterialInterface* HitDecal;

public:

    virtual void Tick(float DeltaTime) override;

    void FireInDirection(const FVector& Direction);

    void FireInDirection(const FVector& Direction, const FVector& Target);

private:

    FVector StartLocation;
    FVector TargetLocation;
    bool bHasTarget = false;
    float ElapsedTime = 0.0f;
    float TravelTime = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float WaveAmplitude = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float WaveFrequency = 6.0f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float LateralAmplitude = 30.0f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float NoiseScale = 1.5f;

    float RandomPhase = 0.0f;
    float RandomAmpFactor = 1.0f;
};
