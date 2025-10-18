// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

};
