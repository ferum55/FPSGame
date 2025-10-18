#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"

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
        CollisionComponent->InitSphereRadius(3.f);
        RootComponent = CollisionComponent;

        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
        CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
        CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
        CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
        CollisionComponent->SetNotifyRigidBodyCollision(true);
        CollisionComponent->SetGenerateOverlapEvents(false);
    }

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
    ProjectileMovementComponent->InitialSpeed = 300.f;
    ProjectileMovementComponent->MaxSpeed = 300.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    InitialLifeSpan = LifeSpan;

    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

    RandomPhase = FMath::FRandRange(0.0f, 2 * PI);
    RandomAmpFactor = FMath::FRandRange(0.75f, 1.35f);
}

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

void AProjectile::OnProjectileStop(const FHitResult& Hit)
{
    SpawnImpactEffect(Hit);
    Destroy();
}

void AProjectile::SpawnImpactEffect(const FHitResult& Hit)
{
    if (HitEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
    }

    if (HitDecal)
    {
        UGameplayStatics::SpawnDecalAtLocation(GetWorld(), HitDecal, FVector(10.0f),
            Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), 5.0f);
    }

}


void AProjectile::FireInDirection(const FVector& Direction)
{
    ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::FireInDirection(const FVector& Direction, const FVector& Target)
{
    StartLocation = GetActorLocation();
    TargetLocation = Target;
    bHasTarget = true;
    ElapsedTime = 0.0f;

    float Distance = FVector::Dist(StartLocation, TargetLocation);
    float Speed = ProjectileMovementComponent->InitialSpeed;
    if (Speed <= KINDA_SMALL_NUMBER)
    {
        Speed = 300.0f;
    }
    TravelTime = FMath::Max(0.01f, Distance / Speed);

    FVector MoveDir = (TargetLocation - StartLocation).GetSafeNormal();
    ProjectileMovementComponent->Velocity = MoveDir * Speed;

    RandomPhase = FMath::FRandRange(0.0f, 2 * PI);
    RandomAmpFactor = FMath::FRandRange(0.75f, 1.35f);
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bHasTarget)
    {
        ElapsedTime += DeltaTime;

        float Alpha = ElapsedTime / TravelTime;
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

        FVector BasePos = FMath::Lerp(StartLocation, TargetLocation, Alpha);
        FVector Forward = (TargetLocation - StartLocation).GetSafeNormal();

        FVector WorldUp = FVector::UpVector;
        if (FMath::Abs(FVector::DotProduct(Forward, WorldUp)) > 0.99f)
        {
            WorldUp = FVector::RightVector;
        }

        FVector Right = FVector::CrossProduct(Forward, WorldUp).GetSafeNormal();
        FVector Up = FVector::CrossProduct(Right, Forward).GetSafeNormal();

        float SinPhase = WaveFrequency * ElapsedTime + RandomPhase;
        float DeterministicOffset = WaveAmplitude * RandomAmpFactor * FMath::Sin(SinPhase);

        float NoiseInput = ElapsedTime * NoiseScale + RandomPhase;
        float NoiseValue = FMath::PerlinNoise1D(NoiseInput);
        float LateralOffset = LateralAmplitude * NoiseValue;

        FVector WaveOffset = Up * DeterministicOffset + Right * LateralOffset;
        FVector NewLocation = BasePos + WaveOffset;

        SetActorLocation(NewLocation, true);

        FVector NewVelocity = ((FMath::Lerp(StartLocation, TargetLocation, FMath::Min(Alpha + 0.01f, 1.0f)) + WaveOffset) - NewLocation) / DeltaTime;
        if (!NewVelocity.ContainsNaN())
        {
            ProjectileMovementComponent->Velocity = NewVelocity;
        }

        if (Alpha >= 1.0f - KINDA_SMALL_NUMBER)
        {
            SetActorLocation(TargetLocation, false);
            FHitResult DummyHit;
            DummyHit.ImpactPoint = TargetLocation;
            DummyHit.ImpactNormal = -Forward;
            SpawnImpactEffect(DummyHit);
            Destroy();
            return;
        }
    }
    else
    {
        if (ProjectileMovementComponent->Velocity.SizeSquared() < 1.0f)
        {
            FHitResult TraceHit;
            FVector Start = GetActorLocation();
            FVector End = Start + GetActorForwardVector() * 100.0f;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            //DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 0.5f);

            if (GetWorld()->LineTraceSingleByChannel(TraceHit, Start, End, ECC_WorldStatic, Params))
            {
                if (HitEffect)
                {
                    FVector SpawnLoc = TraceHit.ImpactPoint + TraceHit.ImpactNormal * 5.0f;
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, SpawnLoc, TraceHit.ImpactNormal.Rotation(), FVector(1.5f));
                }
                if (HitDecal)
                {
                    UGameplayStatics::SpawnDecalAtLocation(GetWorld(), HitDecal, FVector(10.0f),
                        TraceHit.ImpactPoint, TraceHit.ImpactNormal.Rotation(), 5.0f);
                }
                Destroy();
            }
        }
    }
}
