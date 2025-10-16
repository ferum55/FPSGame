// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "DrawDebugHelpers.h"
#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        CollisionComponent->InitSphereRadius(15.f);
        RootComponent = CollisionComponent;

        // Колізія + події
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
        CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
        CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
        CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        CollisionComponent->SetNotifyRigidBodyCollision(true);
        CollisionComponent->SetGenerateOverlapEvents(false);
    }

    // Рух
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
    ProjectileMovementComponent->InitialSpeed = 300.f;
    ProjectileMovementComponent->MaxSpeed = 300.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

    // Mesh
    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    InitialLifeSpan = LifeSpan;

    // Делегати
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse, const FHitResult& Hit)
{


    if (OtherActor != this)
    {
        SpawnImpactEffect(Hit);
        Destroy();
    }
}

// Альтернатива: викликається коли ProjectileMovementComponent зупиняється (навіть якщо OnHit не спрацював)
void AProjectile::OnProjectileStop(const FHitResult& Hit)
{
    SpawnImpactEffect(Hit);
    Destroy();
}

// Загальна функція для спавну ефектів / децаля
void AProjectile::SpawnImpactEffect(const FHitResult& Hit)
{
    // Спробуємо трасування для Foliage
    FHitResult TraceHit;
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 10.f;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(TraceHit, Start, End, ECC_Visibility, Params))
    {
        // Використовуємо результат трасу
        if (HitEffect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, TraceHit.ImpactPoint, TraceHit.ImpactNormal.Rotation(),FVector(0.05f));
        }

        if (HitDecal)
        {
            UGameplayStatics::SpawnDecalAtLocation(
                GetWorld(),
                HitDecal,
                FVector(10.0f),
                TraceHit.ImpactPoint,
                TraceHit.ImpactNormal.Rotation(),
                5.0f
            );
        }
        if (HitNiagaraEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                HitNiagaraEffect,
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation()
            );
        }

    }
    else
    {
        // Якщо трас не знайшов — використовуємо звичайний Hit
        if (HitEffect)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
        }

        if (HitDecal)
        {
            UGameplayStatics::SpawnDecalAtLocation(
                GetWorld(),
                HitDecal,
                FVector(10.0f),
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation(),
                5.0f
            );
        }
        if (HitNiagaraEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                HitNiagaraEffect,
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation()
            );
        }
        SetLifeSpan(0.1f);
    }
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ProjectileMovementComponent->Velocity.SizeSquared() < 1.0f)
    {
        FHitResult TraceHit;
        FVector Start = GetActorLocation();
        FVector End = Start + GetActorForwardVector() * 100.0f; // збільшив відстань
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        // 🔴 debug-лінія щоб бачити трас
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 0.5f);

        // ⚙️ зміни: тимчасово перевір ECC_WorldStatic
        if (GetWorld()->LineTraceSingleByChannel(TraceHit, Start, End, ECC_WorldStatic, Params))
        {

            if (HitEffect)
            {
                // трохи відсунемо ефект від поверхні
                FVector SpawnLoc = TraceHit.ImpactPoint + TraceHit.ImpactNormal * 5.0f;
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, SpawnLoc, TraceHit.ImpactNormal.Rotation(), FVector(1.5f));
            }
            else
            {
            }

            if (HitDecal)
            {
                UGameplayStatics::SpawnDecalAtLocation(GetWorld(), HitDecal, FVector(10.0f),
                    TraceHit.ImpactPoint, TraceHit.ImpactNormal.Rotation(), 5.0f);
            }

            Destroy(); // ✅ переносимо сюди
        }

    }
}


void AProjectile::FireInDirection(const FVector& Direction)
{
    ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}
