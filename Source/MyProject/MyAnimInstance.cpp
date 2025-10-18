#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* PawnOwner = TryGetPawnOwner();
    if (!PawnOwner)
    {
        return;
    }

    FRotator ControlRot = PawnOwner->GetControlRotation();
    AimPitch = FRotator::NormalizeAxis(ControlRot.Pitch);

}
