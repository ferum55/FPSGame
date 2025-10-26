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

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    // Створюємо віджет-компонент
    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawAtDesiredSize(true);
    HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); // над головою
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();

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

    FVector NewLoc = FMath::VInterpConstantTo(MyLoc, TargetLoc, DeltaSeconds, MoveSpeed);
    SetActorLocation(NewLoc);

    FRotator LookRot = Dir.Rotation();
    SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));
}

void AEnemy::SetHealth(float NewHealth)
{
    CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);

    if (UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject())
    {
        if (UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("EnemyHealthBar"))))
        {
            Bar->SetPercent(CurrentHealth / MaxHealth);
        }
    }

    if (CurrentHealth <= 0.f)
    {
        Destroy();
    }
}
