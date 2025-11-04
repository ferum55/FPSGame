#include "AmmoPickup.h"
#include "MainCharacter.h"

void AAmmoPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
	{
		Player->AddAmmo(AmmoAmount);
		Destroy();
	}
}
