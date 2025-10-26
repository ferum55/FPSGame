// MyProjectGameModeBase.cpp

#include "MyProjectGameModeBase.h"
#include "MainCharacter.h"
#include "FPSHUD.h"
#include "myHUD.h"


#include "UObject/ConstructorHelpers.h"

AMyProjectGameModeBase::AMyProjectGameModeBase()
{
    DefaultPawnClass = AMainCharacter::StaticClass();

    /*static ConstructorHelpers::FClassFinder<AHUD> HUD_BP(TEXT("/Game/UI/BP_HUD.BP_HUD_C"));
    if (HUD_BP.Succeeded())
    {
        HUDClass = HUD_BP.Class;
    }*/
}

