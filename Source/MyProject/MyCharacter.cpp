// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "MyAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

namespace
{
    constexpr float kMinPitchDeg = -85.f; 
    constexpr float kMaxPitchDeg = 80.f; 
}

static void HideHeadBonesAuto(USkeletalMeshComponent* Mesh)
{
    if (!Mesh) return;

    const TCHAR* Candidates[] =
    {
        TEXT("head"), TEXT("Head"), TEXT("head_01"),
        TEXT("neck"), TEXT("neck_01"),
        TEXT("b_Head"), TEXT("Bip001-Head")
    };

    for (auto* Name : Candidates)
    {
        if (Mesh->GetBoneIndex(Name) != INDEX_NONE)
        {
            Mesh->HideBoneByName(Name, EPhysBodyOp::PBO_None);
            UE_LOG(LogTemp, Warning, TEXT("Hidden bone by name: %s"), Name);
            break; 
        }
    }
}


AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->DefaultLandMovementMode = MOVE_Walking;
        Move->SetMovementMode(MOVE_Walking);
        Move->GravityScale = 1.f;
        Move->GetNavAgentPropertiesRef().bCanCrouch = true;
        Move->bCrouchMaintainsBaseLocation = true;

        Move->CrouchedHalfHeight = 44.f;
        Move->MaxWalkSpeed = 300.f;
        Move->MaxWalkSpeedCrouched = 200.f;
        Move->JumpZVelocity = 400.f;
        Move->AirControl = 0.35f;

        bUseControllerRotationYaw = true;
        Move->bOrientRotationToMovement = false;
    }


    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    check(Camera);
    Camera->SetupAttachment(GetCapsuleComponent());

    Camera->SetRelativeLocation(FVector(30.f, 0.f, 60.f));
    Camera->bUsePawnControlRotation = true;


    USkeletalMeshComponent* Body = GetMesh();
    Body->SetupAttachment(GetMesh(), TEXT("head"));
    Body->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
    Body->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Body->SetOwnerNoSee(false);      
    Body->bCastHiddenShadow = false;
    Body->CastShadow = false;

    if (!Body->SkeletalMesh)
    {
        static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyMeshRef(
            TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
        if (BodyMeshRef.Succeeded())
        {
            Body->SetSkeletalMesh(BodyMeshRef.Object);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Body mesh not found: /Game/Mannequin/Character/Mesh/SK_Mannequin"));
        }
    }
    // --- ЦЕ ПІДСТАВИТИ НАТОМІСТЬ ---
    static ConstructorHelpers::FClassFinder<UAnimInstance> ASPAnimBP(
        TEXT("/Game/AnimRetargeted/UE4ASP_HeroTPP_AnimBlueprint.UE4ASP_HeroTPP_AnimBlueprint_C")
    );
    if (ASPAnimBP.Succeeded())
    {
        BodyAnimClass = ASPAnimBP.Class; // <— тепер не nullptr
        USkeletalMeshComponent* MeshComp = GetMesh();
        MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        MeshComp->SetAnimInstanceClass(BodyAnimClass);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load UE4ASP_HeroTPP_AnimBlueprint"));
    }



    ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowMesh"));
    ShadowMesh->SetupAttachment(GetCapsuleComponent());
    ShadowMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
    ShadowMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    ShadowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    ShadowMesh->SetHiddenInGame(true);      
    ShadowMesh->bCastHiddenShadow = true;   
}


void AMyCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (USkeletalMeshComponent* M = GetMesh())
    {
        // На випадок, якщо конструктор був замінений налаштуваннями ззовні
        if (!M->AnimClass && BodyAnimClass)
        {
            M->SetAnimationMode(EAnimationMode::AnimationBlueprint);
            M->SetAnimInstanceClass(BodyAnimClass);
        }

        // 2) Гарантуємо створення інстанса анімбп
        if (!M->GetAnimInstance())
        {
            M->InitializeAnimScriptInstance();
        }

        UE_LOG(LogTemp, Warning, TEXT("[PostInit] AnimClass=%s AnimInstance=%s"),
            *GetNameSafe(M->AnimClass), *GetNameSafe(M->GetAnimInstance()));
    }

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->GetNavAgentPropertiesRef().bCanCrouch = true;
        Move->bCrouchMaintainsBaseLocation = true;

        Move->DefaultLandMovementMode = MOVE_Walking;
        Move->SetMovementMode(MOVE_Walking);

        bUseControllerRotationYaw = true;
        Move->bOrientRotationToMovement = false;
    }

    if (Camera)
    {
        /*Camera->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);*/
        Camera->SetRelativeLocation(FVector(20.f, 0.f, 70.f));
        Camera->SetRelativeLocation(FVector(0.f, 10.f, 10.f)); // трохи вперед і вниз
        Camera->bUsePawnControlRotation = true;
    }

    if (USkeletalMeshComponent* Body = GetMesh())
    {
        Body->SetOwnerNoSee(false);
        Body->bCastHiddenShadow = true;
        Body->CastShadow = true;
        Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (USkeletalMeshComponent* Body = GetMesh())
    {
        if (ShadowMesh)
        {
            ShadowMesh->SetSkeletalMesh(GetMesh()->SkeletalMesh);
            ShadowMesh->SetAnimInstanceClass(GetMesh()->AnimClass);
            ShadowMesh->SetHiddenInGame(true);
            ShadowMesh->bCastHiddenShadow = true;
            ShadowMesh->CastShadow = true;
        }
    }
    if (USkeletalMeshComponent* M = GetMesh())
    {
        M->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        M->SetAnimInstanceClass(BodyAnimClass);
        M->InitializeAnimScriptInstance();
    }

}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

#include "Animation/AnimBlueprintGeneratedClass.h"

    if (USkeletalMeshComponent* M = GetMesh())
    {
        const USkeleton* MeshSkel = M->SkeletalMesh ? M->SkeletalMesh->GetSkeleton() : nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Mesh Skeleton: %s (%s)"),
            *GetNameSafe(MeshSkel),
            MeshSkel ? *MeshSkel->GetPathName() : TEXT("None"));

        if (UClass* Cls = M->AnimClass)
        {
            if (auto* Gen = Cast<UAnimBlueprintGeneratedClass>(Cls))
            {
                const USkeleton* AnimSkel = Gen->TargetSkeleton;
                UE_LOG(LogTemp, Warning, TEXT("AnimBP TargetSkeleton: %s (%s)"),
                    *GetNameSafe(AnimSkel),
                    AnimSkel ? *AnimSkel->GetPathName() : TEXT("None"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AnimClass is not UAnimBlueprintGeneratedClass"));
            }
        }
    }

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->SetMovementMode(MOVE_Walking);
        
    }

    if (IsLocallyControlled() && GetMesh())
    {
        HideHeadBonesAuto(GetMesh());
    }

    GetMesh()->SetHiddenInGame(false);

   /* UE_LOG(LogTemp, Warning, TEXT("OwnerNoSee=%d CastShadow=%d bCastHiddenShadow=%d"),
        GetMesh()->bOwnerNoSee, GetMesh()->bCastHiddenShadow);

    UE_LOG(LogTemp, Warning, TEXT("ShadowMesh: HiddenInGame=%d CastShadow=%d bCastHiddenShadow=%d SkeletalMesh=%s"),
        ShadowMesh->bHiddenInGame, ShadowMesh->CastShadow, ShadowMesh->bCastHiddenShadow,
        *GetNameSafe(ShadowMesh->SkeletalMesh));*/


    if (USkeletalMeshComponent* M = GetMesh())
    {
        UE_LOG(LogTemp, Warning, TEXT("AnimInstance: %s"), *GetNameSafe(M->GetAnimInstance()));
        UE_LOG(LogTemp, Warning, TEXT("Skeleton on Mesh: %s"), *GetNameSafe(M->SkeletalMesh ? M->SkeletalMesh->GetSkeleton() : nullptr));
        UE_LOG(LogTemp, Warning, TEXT("bPauseAnims=%d GlobalAnimRateScale=%.2f"),
            M->bPauseAnims, M->GlobalAnimRateScale);

        // на час діагностики гарантуємо тік анімацій
        M->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    }


}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
   /* UE_LOG(LogTemp, Warning, TEXT("[Tick] Vel=%s Speed=%f"),
        *GetVelocity().ToString(),
        GetVelocity().Size2D());*/
    //UE_LOG(LogTemp, Warning, TEXT("MovementMode=%d"), (int32)GetCharacterMovement()->MovementMode);

    FRotator ControlRot = GetControlRotation();
    float Pitch = ControlRot.Pitch;

    // Доступ до скелетної сітки
    if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
    {
        // Отримуємо нашу кастомну AnimInstance
        if (UMyAnimInstance* AnimInst = Cast<UMyAnimInstance>(SkeletalMeshComp->GetAnimInstance()))
        {
            AnimInst->AimPitch = Pitch;
        }
    }

}


void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyCharacter::EndCrouch);
}

void AMyCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMyCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}



void AMyCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMyCharacter::LookUp(float Value)
{
    if (!Controller) return;

    AddControllerPitchInput(Value);

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        FRotator R = PC->GetControlRotation();
        R.Pitch = FMath::ClampAngle(R.Pitch, kMinPitchDeg, kMaxPitchDeg);
        PC->SetControlRotation(R);
    }
}

void AMyCharacter::BeginCrouch()
{
    UE_LOG(LogTemp, Warning, TEXT("Crouch PRESSED"));
    if (CanCrouch())
    {
        Crouch();
    }
}

void AMyCharacter::EndCrouch()
{
    UnCrouch();
}

void AMyCharacter::OnJumpPressed()
{
    JumpButtonDown = true;
    Jump(); // <-- це стандартна функція із базового ACharacter
}

void AMyCharacter::OnJumpReleased()
{
    JumpButtonDown = false;
    StopJumping(); // <-- теж стандартна функція ACharacter
}

