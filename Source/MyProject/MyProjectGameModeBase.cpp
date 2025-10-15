// MyProjectGameModeBase.cpp

#include "MyProjectGameModeBase.h"
#include "MainCharacter.h"
#include "FPSHUD.h"


#include "UObject/ConstructorHelpers.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
{
    DefaultPawnClass = AMainCharacter::StaticClass();

    static ConstructorHelpers::FClassFinder<AHUD> HUD_BP(TEXT("/Game/BP_FPSHUD.BP_FPSHUD_C"));
    if (HUD_BP.Succeeded())
    {
        HUDClass = HUD_BP.Class;
    }
}

