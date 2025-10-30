#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpawnEnemyTargetPoint.h"
#include "Components/WidgetComponent.h"
#include "TimerManager.h"
#include "Enemy.generated.h"

class AProjectile;


UCLASS()
class MYPROJECT_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    UFUNCTION(BlueprintCallable)
    void SetHealth(float NewHealth);
    float GetHealth() const { return CurrentHealth; }
    UFUNCTION(BlueprintImplementableEvent)
    void PlayHitReaction_BP();


protected:
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 200.f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<ASpawnEnemyTargetPoint*> PatrolPoints;

    int32 CurrentPatrolIndex;

    void MoveAlongPatrol(float DeltaTime);

    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HealthBarWidget;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 800.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackCooldown = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ProjectileSpeed = 1000.f;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* DeathAnim;


    bool bCanAttack = true;
    bool bIsAttacking = false;

    FTimerHandle AttackCooldownHandle;

    void TryAttack();
    void PerformAttack();
    void ResetAttack();
    void SpawnProjectileAtPlayer();


};
