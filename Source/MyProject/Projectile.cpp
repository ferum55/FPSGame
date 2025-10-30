// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Enemy.h"
#include "MainCharacter.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}
	if (!CollisionComponent) {
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->InitSphereRadius(15.0f);
		RootComponent = CollisionComponent;
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 300.f;
	ProjectileMovementComponent->MaxSpeed = 300.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->Bounciness = 0.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = LifeSpan;
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->BodyInstance.bUseCCD = true;
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	if (!CollisionComponent) {
		UE_LOG(LogTemp, Error, TEXT("No CollisionComponent!"));
		return;
	}

	
}



void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Loc = GetActorLocation();
	UE_LOG(LogTemp, VeryVerbose, TEXT("Projectile at: %s"), *Loc.ToString());


}

void AProjectile::FireInDirection(const FVector& Direction) {
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	AController* InstigatorCtrl = GetInstigatorController();
	UE_LOG(LogTemp, Warning, TEXT("[Projectile] %s fired by %s hitting %s"),
		*GetName(),
		InstigatorCtrl ? *InstigatorCtrl->GetName() : TEXT("NoController"),
		OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	if (OtherActor && OtherActor != this)
	{
		if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile calling TakeDamage on player"));
			Player->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
		}
		else if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
		{
			float OldHP = Enemy->GetHealth();
			Enemy->SetHealth(OldHP - Damage);
		}
	}

	static const float Impulse = 100.0f;
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics()) {
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * Impulse, Hit.ImpactPoint);

	}
	Destroy();
}


