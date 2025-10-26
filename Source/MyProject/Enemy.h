#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpawnEnemyTargetPoint.h"
#include "Components/WidgetComponent.h"
#include "Enemy.generated.h"

UCLASS()
class MYPROJECT_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // --- ַהמנמג’ÿ ---
    UFUNCTION(BlueprintCallable)
    void SetHealth(float NewHealth);
    float GetHealth() const { return CurrentHealth; }

protected:
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 0.f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<ASpawnEnemyTargetPoint*> PatrolPoints;

    int32 CurrentPatrolIndex;

    void MoveAlongPatrol(float DeltaTime);

    // --- ֲ³הזוע ---
    UPROPERTY(VisibleAnywhere, Category = "UI")
    UWidgetComponent* HealthBarWidget;
};
