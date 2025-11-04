#include "HealthPickup.h"
#include "MainCharacter.h"

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		Player->AddHealth(HealAmount);
		Destroy();
	}
}
