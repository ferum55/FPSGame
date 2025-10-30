// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "myHUD.h"
#include "Projectile.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class USkeletalMesh;
class UAnimInstance;


UCLASS()
class MYPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AMainCharacter();


protected:

	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Character|Animation")
	TSubclassOf<UAnimInstance> BodyAnimClass; 
	UPROPERTY(EditDefaultsOnly, Category = "Character|Mesh")
	USkeletalMesh* BodyMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AActor* CurrentWeapon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UmyHUD> HUDClass;

	// Створений екземпляр HUD
	UPROPERTY()
	UmyHUD* HUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxAmmo = 30;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool JumpButtonDown;


public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION() void MoveForward(float Value);
	UFUNCTION() void MoveRight(float Value);
	UFUNCTION() void Turn(float Value);
	UFUNCTION() void LookUp(float Value);
	UFUNCTION() void BeginCrouch();
	UFUNCTION() void EndCrouch();
	UFUNCTION() void OnJumpPressed();
	UFUNCTION() void OnJumpReleased();
	UFUNCTION() void Fire();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;


};
