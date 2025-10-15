
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"


class UCameraComponent;
class USkeletalMeshComponent;

UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;


	// MyCharacter.h Ч можна залишити це поле
	UPROPERTY(EditDefaultsOnly, Category = "Character|Animation")
	TSubclassOf<UAnimInstance> BodyAnimClass = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(EditDefaultsOnly, Category = "Character|Meshes") USkeletalMesh* BodyMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* ShadowMesh;
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinPitch = -85.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxPitch = 85.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	bool JumpButtonDown = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	float AimPitch = 0.0f;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;


	UFUNCTION() void MoveForward(float Value);
	UFUNCTION() void MoveRight(float Value);
	UFUNCTION() void Turn(float Value);
	UFUNCTION() void LookUp(float Value);
	UFUNCTION() void BeginCrouch();
	UFUNCTION() void EndCrouch();
	UFUNCTION() void OnJumpPressed();
	UFUNCTION() void OnJumpReleased();
};
