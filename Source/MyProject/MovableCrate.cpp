#include "MovableCrate.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AMovableCrate::AMovableCrate()
{
	PrimaryActorTick.bCanEverTick = false;

	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
	RootComponent = CrateMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshAsset.Succeeded())
	{
		CrateMesh->SetStaticMesh(MeshAsset.Object);
	}

	CrateMesh->SetSimulatePhysics(true);
	CrateMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	CrateMesh->SetMassOverrideInKg(NAME_None, 50.0f, true);
	CrateMesh->SetLinearDamping(0.5f);
	CrateMesh->SetAngularDamping(1.0f);
	CrateMesh->BodyInstance.bUseCCD = true;

}
void AMovableCrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMovableCrate::BeginPlay()
{
	Super::BeginPlay();
}
