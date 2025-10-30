#include "Enemy.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentPatrolIndex = 0;
    CurrentHealth = MaxHealth;

    // --- Налаштування руху ---
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);



    GetMesh()->SetupAttachment(GetCapsuleComponent());
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(GetMesh(), TEXT("head")); // ? прив’язка до кістки голови
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // невеликий зсув над головою
    HealthBarWidget->SetPivot(FVector2D(0.5f, 1.0f));
    HealthBarWidget->SetDrawSize(FVector2D(200.f, 40.f));

}


void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("MovementMode: %d"), (int)GetCharacterMovement()->MovementMode);
    if (!Controller)
    {
        SpawnDefaultController();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy already has controller: %s"), *Controller->GetName());
    }

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // Якщо є клас віджета у Blueprint
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("EnemyHealth"))))
        {
            Bar->SetPercent(CurrentHealth / MaxHealth);
        }
    }


    // Патруль
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnEnemyTargetPoint::StaticClass(), Found);
    for (AActor* P : Found)
        if (auto* TP = Cast<ASpawnEnemyTargetPoint>(P)) PatrolPoints.Add(TP);

    UCapsuleComponent* Cap = GetCapsuleComponent();
    /*UE_LOG(LogTemp, Warning, TEXT("Enemy capsule SimulatePhysics=%d, NotifyRigidBodyCollision=%d, CollisionEnabled=%d, ResponseToProjectile=%d"),
        Cap->IsSimulatingPhysics(),
        Cap->BodyInstance.bNotifyRigidBodyCollision,
        (int)Cap->GetCollisionEnabled(),
        (int)Cap->GetCollisionResponseToChannel(ECC_GameTraceChannel1));*/

}

void AEnemy::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UE_LOG(LogTemp, Warning, TEXT("[Enemy] Tick: Attacking=%d CanAttack=%d Speed=%.1f"),
        bIsAttacking ? 1 : 0, bCanAttack ? 1 : 0, GetVelocity().Size());

    if (!bIsAttacking)                     // <-- стопимо патруль під час атаки
        MoveAlongPatrol(DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
    {
        float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        UE_LOG(LogTemp, Warning, TEXT("[Enemy] DistToPlayer=%.1f"), Dist);

        if (Dist <= AttackRange && bCanAttack && !bIsAttacking) // не тригерити атаку, якщо вже атакуємо
            TryAttack();
    }
}


void AEnemy::MoveAlongPatrol(float DeltaSeconds)
{
    if (PatrolPoints.Num() == 0) return;

    AActor* Target = PatrolPoints[CurrentPatrolIndex];
    if (!Target) return;

    FVector MyLoc = GetActorLocation();
    FVector TargetLoc = Target->GetActorLocation();

    FVector Dir = (TargetLoc - MyLoc);
    float Dist = Dir.Size();

    if (Dist < 80.f)
    {
        CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
        return;
    }

    Dir.Normalize();

    AddMovementInput(Dir, 1.0f);


    FRotator LookRot = Dir.Rotation();
    SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));
}

void AEnemy::SetHealth(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("EnemyHealth"))))
        {
            Bar->SetPercent(CurrentHealth / MaxHealth);
        }
    }


    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        /*if (!(UAnimInstance* Anim = MeshComp->GetAnimInstance()))
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy has NO AnimInstance!"));
        }*/
    }


    UE_LOG(LogTemp, Warning, TEXT("SetHealth called. Old=%.1f New=%.1f"), OldHealth, CurrentHealth);

    if (CurrentHealth > 0.f)
    {
        PlayHitReaction_BP();          // звичайна реакція
    }
    else
    {
        if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
        {
            Anim->Montage_Stop(0.1f);  // зупинити всі інші анімації
            GetMesh()->PlayAnimation(DeathAnim, false); // ? DeathAnim = Enemy_Death
        }

        GetCharacterMovement()->DisableMovement();
        SetActorEnableCollision(false);

        FTimerHandle DeathHandle;
        GetWorldTimerManager().SetTimer(DeathHandle, [this]()
            {
                Destroy();
            }, 3.0f, false);
    }

}

void AEnemy::TryAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("[Enemy] TryAttack()"));
    bIsAttacking = true;            // <-- додати
    bCanAttack = false;

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->MaxWalkSpeed = 0.f;
    GetCharacterMovement()->bOrientRotationToMovement = false; // щоб не крутило за вектором руху

    if (APawn* P = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        FVector dir = P->GetActorLocation() - GetActorLocation(); dir.Z = 0;
        SetActorRotation(dir.Rotation());
    }

    PerformAttack();
    GetWorldTimerManager().SetTimer(AttackCooldownHandle, this, &AEnemy::ResetAttack, AttackCooldown, false);
}


void AEnemy::PerformAttack()
{
    // Зупинка руху
    GetCharacterMovement()->StopMovementImmediately();

    // Поворот у бік гравця
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
    {
        FVector Dir = PlayerPawn->GetActorLocation() - GetActorLocation();
        Dir.Z = 0;
        SetActorRotation(Dir.Rotation());
    }

    // Активуємо стан атаки в AnimBP
    if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
    {
        FBoolProperty* Prop = FindFProperty<FBoolProperty>(Anim->GetClass(), FName("IsAttacking"));
        if (Prop) Prop->SetPropertyValue_InContainer(Anim, true);
    }

    // Затримка перед пострілом (синхрон із анімацією)
    GetWorldTimerManager().SetTimerForNextTick(this, &AEnemy::SpawnProjectileAtPlayer);

    // Через короткий час повертаємо Idle
    FTimerHandle ResetAnimHandle;
    GetWorldTimerManager().SetTimer(ResetAnimHandle, [this]()
        {
            if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
            {
                FBoolProperty* Prop = FindFProperty<FBoolProperty>(Anim->GetClass(), FName("IsAttacking"));
                if (Prop) Prop->SetPropertyValue_InContainer(Anim, false);
            }
        }, 0.9f, false); // тривалість атаки
}


void AEnemy::ResetAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("[Enemy] ResetAttack()"));
    bCanAttack = true;
    bIsAttacking = false;                          // <-- додати
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}




void AEnemy::SpawnProjectileAtPlayer()
{
    if (!ProjectileClass) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!PlayerPawn) return;

    FVector Start = GetActorLocation() + GetActorForwardVector() * 80.f + FVector(0, 0, 50.f);
    FRotator Rot = (PlayerPawn->GetActorLocation() - Start).Rotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;

    AProjectile* P = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, Start, Rot, Params);
    if (P)
    {
        if (auto* MoveComp = P->FindComponentByClass<UProjectileMovementComponent>())
        {
            MoveComp->InitialSpeed = ProjectileSpeed;
            MoveComp->Velocity = Rot.Vector() * ProjectileSpeed;
        }
    }
}


