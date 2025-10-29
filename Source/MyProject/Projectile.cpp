// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Enemy.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	/*CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);*/
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->BodyInstance.bUseCCD = true; // для швидких снарядів
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


	// Перевір усі відповіді на канали
	// Отримати всі відповіді на канали
	FCollisionResponseContainer Responses = CollisionComponent->GetCollisionResponseToChannels();

	
}


// Called every frame
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
	UE_LOG(LogTemp, Error, TEXT("ON HIT WAS CALLED! Target: %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	if (OtherActor && OtherActor != this)
	{
		class AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			float OldHP = Enemy->GetHealth();
			float Damage = 20.f; // або винеси в UPROPERTY(EditAnywhere)
			Enemy->SetHealth(OldHP - Damage);

			UE_LOG(LogTemp, Warning, TEXT("Hit enemy! HP: %.1f -> %.1f"), OldHP, Enemy->GetHealth());
		}
	}
	static const float Impulse = 100.0f;
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics()) {
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * Impulse, Hit.ImpactPoint);

	}
	Destroy();
}


