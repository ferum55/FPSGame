// Fill out your copyright notice in the Description page of Project Settings.


#include "TeleportTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"

// Sets default values
ATeleportTrigger::ATeleportTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TeleportZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TeleportZone"));
	RootComponent = TeleportZone;
	TeleportZone->InitBoxExtent(FVector(100.f, 100.f, 100.f));
	TeleportZone->SetCollisionProfileName(TEXT("Trigger"));
	TeleportZone->SetGenerateOverlapEvents(true);

	TeleportTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("TeleportTarget"));
	TeleportTarget->SetupAttachment(RootComponent);

	TeleportZone->OnComponentBeginOverlap.AddDynamic(this, &ATeleportTrigger::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ATeleportTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (PortalEffect)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PortalEffect, GetActorLocation());

}

void ATeleportTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherCOmp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherActor) return;
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter) {
		FVector TargetLocation = TeleportTarget->GetComponentLocation();
		FRotator TargetRotation = TeleportTarget->GetComponentRotation();

		PlayerCharacter->TeleportTo(TargetLocation, TargetRotation);
	}
}

// Called every frame
void ATeleportTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

