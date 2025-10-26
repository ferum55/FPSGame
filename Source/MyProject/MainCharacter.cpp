// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
//
//#include "Projectile.h"
#include "MyAnimInstance.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Тіло персонажа
    GetMesh()->SetupAttachment(GetCapsuleComponent());
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Завантажуємо модель тіла (Mannequin)
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> BodyMeshRef(
        TEXT("/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin")
    );
    if (BodyMeshRef.Succeeded())
    {
        BodyMesh = BodyMeshRef.Object;
        GetMesh()->SetSkeletalMesh(BodyMesh);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Body mesh not found"));
    }

    // Завантажуємо анімаційний Blueprint
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
        TEXT("/Game/AnimRetargeted/BP_AnimMainCharacter.BP_AnimMainCharacter_C")
    );
    if (AnimBPClass.Succeeded())
    {
        BodyAnimClass = AnimBPClass.Class;
        GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        GetMesh()->SetAnimInstanceClass(BodyAnimClass);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to load AnimBlueprint"));
    }

    // ---------- FP_Mesh (видимий тільки гравцю) ----------
    FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Mesh"));
    FP_Mesh->SetupAttachment(GetMesh());
    FP_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // видно лише локальному гравцю
    FP_Mesh->SetOnlyOwnerSee(true);
    FP_Mesh->bCastHiddenShadow = false;
    FP_Mesh->CastShadow = false;

    // копіюємо сітку й анімацію з основного Mesh
    FP_Mesh->SetSkeletalMesh(GetMesh()->SkeletalMesh);
    FP_Mesh->SetAnimInstanceClass(GetMesh()->AnimClass);


    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    Camera->SetupAttachment(FP_Mesh, TEXT("camera"));
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    Camera->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    Camera->bUsePawnControlRotation = true;

    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
    GetCharacterMovement()->bCrouchMaintainsBaseLocation = true;
    static ConstructorHelpers::FClassFinder<AActor> WeaponBP(
        TEXT("/Game/FPS_Weapon_Bundle/BP_KA74U.BP_KA74U_C") // ⚠️ твій реальний шлях до Blueprint зброї
    );
    if (WeaponBP.Succeeded())
    {
        WeaponClass = WeaponBP.Class;
    }
    static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBP(TEXT("/Game/Projectile/BP_Projectile.BP_Projectile_C"));
    if (ProjectileBP.Succeeded())
    {
        ProjectileClass = ProjectileBP.Class;
    }

    static ConstructorHelpers::FClassFinder<UmyHUD> HUD_BP(TEXT("/Game/UI/BP_HUD.BP_HUD_C"));
    if (HUD_BP.Succeeded())
    {
        HUDClass = HUD_BP.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("❌ Failed to find BP_HUD at /Game/UI/BP_HUD.BP_HUD_C"));
    }


}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();


    // У BeginPlay:
    if (IsLocallyControlled())
    {
        // Голову не показуємо на екрані
        FP_Mesh->HideBoneByName(TEXT("head"), EPhysBodyOp::PBO_None);
        FP_Mesh->HideBoneByName(TEXT("neck_01"), EPhysBodyOp::PBO_None);

        // Але залишаємо тінь
        GetMesh()->CastShadow = true;
        GetMesh()->bCastHiddenShadow = true;
        GetMesh()->SetOwnerNoSee(true);
    }

    if (WeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;

        // Створюємо зброю
        CurrentWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, SpawnParams);

        if (CurrentWeapon)
        {
            // Прикріплюємо до сокета на FP_Mesh (або GetMesh, залежно від вигляду)
            CurrentWeapon->AttachToComponent(
                FP_Mesh,
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                TEXT("weapon_socket") // назва твого сокета
            );
        }
    }

    if (HUDClass)
    {
        HUDWidget = CreateWidget<UmyHUD>(GetWorld(), HUDClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("HUD added to viewport"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HUDWidget is null"));
        }
    }else UE_LOG(LogTemp, Error, TEXT("HUI"));


}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (FP_Mesh && FP_Mesh->GetAnimInstance())
    {
        UMyAnimInstance* Anim = Cast<UMyAnimInstance>(FP_Mesh->GetAnimInstance());
        if (Anim && Controller)
        {
            const FRotator ControlRot = Controller->GetControlRotation();
            Anim->AimPitch = FRotator::NormalizeAxis(ControlRot.Pitch);
        }
    }

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::OnJumpPressed);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::OnJumpReleased);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMainCharacter::EndCrouch);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::Fire);
}


void AMainCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMainCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMainCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMainCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMainCharacter::BeginCrouch()
{
    if (CanCrouch()) {
        Crouch();

    }
        

}

void AMainCharacter::EndCrouch()
{
    UnCrouch();
}

void AMainCharacter::OnJumpPressed()
{
    Jump();
    JumpButtonDown = true;
}

void AMainCharacter::OnJumpReleased()
{
    StopJumping();
    JumpButtonDown = false;
}



void AMainCharacter::Fire()
{

    if (Ammo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Out of ammo!"));
        return;
    }

    FVector CameraLocation = Camera->GetComponentLocation();
    FRotator CameraRotation = Camera->GetComponentRotation();
    FVector ShootDirection = CameraRotation.Vector();

    FVector MuzzleLocation = CameraLocation + CameraRotation.Vector() * 150.f + FTransform(CameraRotation).TransformVector(MuzzleOffset);

    FRotator MuzzleRotation = CameraRotation;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
    if (Projectile)
    {
        /*UE_LOG(LogTemp, Warning, TEXT("Projectile Fired!"));*/
        FVector LaunchDirection = MuzzleRotation.Vector();
        Projectile->FireInDirection(LaunchDirection);
    }

    Ammo = FMath::Clamp(Ammo - 1, 0, MaxAmmo);

    if (HUDWidget)
    {
        
        HUDWidget->SetAmmo(Ammo);
    }
}




