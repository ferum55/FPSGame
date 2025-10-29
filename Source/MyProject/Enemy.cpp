#include "Enemy.h"
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


    // --- Правильне розташування Mesh ---
    // У ACharacter Mesh має бути прикріплений до CapsuleComponent
    GetMesh()->SetupAttachment(GetCapsuleComponent());
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // --- Віджет здоров’я ---
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(GetMesh()); // було RootComponent ? виправляємо!
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}


void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("MovementMode: %d"), (int)GetCharacterMovement()->MovementMode);
    if (!Controller)
    {
        SpawnDefaultController();
        UE_LOG(LogTemp, Warning, TEXT("Controller spawned for enemy."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy already has controller: %s"), *Controller->GetName());
    }

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    UE_LOG(LogTemp, Warning, TEXT("Enemy MoveSpeed applied = %.1f"), MoveSpeed);

    // Якщо є клас віджета у Blueprint
    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("EnemyHealthBar"))))
        {
            Bar->SetPercent(1.0f);
        }
    }

    // Патруль
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnEnemyTargetPoint::StaticClass(), Found);
    for (AActor* P : Found)
        if (auto* TP = Cast<ASpawnEnemyTargetPoint>(P)) PatrolPoints.Add(TP);

    UCapsuleComponent* Cap = GetCapsuleComponent();
    UE_LOG(LogTemp, Warning, TEXT("Enemy capsule SimulatePhysics=%d, NotifyRigidBodyCollision=%d, CollisionEnabled=%d, ResponseToProjectile=%d"),
        Cap->IsSimulatingPhysics(),
        Cap->BodyInstance.bNotifyRigidBodyCollision,
        (int)Cap->GetCollisionEnabled(),
        (int)Cap->GetCollisionResponseToChannel(ECC_GameTraceChannel1));

}

void AEnemy::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    MoveAlongPatrol(DeltaSeconds);
    FVector Vel = GetVelocity();
    float Speed = Vel.Size();

    UE_LOG(LogTemp, Warning, TEXT("Enemy speed: %.1f"), Speed);

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
        {
            UE_LOG(LogTemp, Warning, TEXT("AnimInstance: %s"), *Anim->GetClass()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No AnimInstance on Mesh!"));
        }
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
    UE_LOG(LogTemp, Warning, TEXT("Dir: %s"), *Dir.ToString());


    UE_LOG(LogTemp, Warning, TEXT("Moving to target %s"), *Target->GetName());

    AddMovementInput(Dir, 1.0f);


    FRotator LookRot = Dir.Rotation();
    SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));
}

void AEnemy::SetHealth(float NewHealth)
{
    float OldHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy AnimInstance: %s"),
                *Anim->GetClass()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy has NO AnimInstance!"));
        }
    }


    UE_LOG(LogTemp, Warning, TEXT("SetHealth called. Old=%.1f New=%.1f"), OldHealth, CurrentHealth);

    if (CurrentHealth > 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy still alive — calling PlayHitReaction_BP()"));
        PlayHitReaction_BP();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy died — Destroy() called"));
        Destroy();
    }
}

