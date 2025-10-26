#include "myHUD.h"
#include "Components/ProgressBar.h"

void UmyHUD::SetAmmo(int32 CurrentAmmo)
{
    if (!AmmoBar)
    {
        UE_LOG(LogTemp, Error, TEXT("? AmmoBar is null in HUD!"));
        return;
    }

    float Percent = FMath::Clamp((float)CurrentAmmo / 30.0f, 0.f, 1.f);
    AmmoBar->SetPercent(Percent);

   /* UE_LOG(LogTemp, Warning, TEXT("? SetAmmo updated bar to %.2f"), Percent);*/
}

void UmyHUD::SetHealth(float CurrentHealth)
{
    if (!HealthBar)
    {
        UE_LOG(LogTemp, Error, TEXT("? HealthBar is null in HUD!"));
        return;
    }

    float Percent = FMath::Clamp(CurrentHealth / 100.f, 0.f, 1.f);
    HealthBar->SetPercent(Percent);

    // плавна зміна кольору: червоний ? зелений
    FLinearColor FullColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
    FLinearColor LowColor = FLinearColor(1.f, 0.f, 0.f, 1.f);
    FLinearColor NewColor = FMath::Lerp(LowColor, FullColor, Percent);

    HealthBar->SetFillColorAndOpacity(NewColor);

    UE_LOG(LogTemp, Warning, TEXT("? SetHealth updated bar to %.2f"), Percent);
}
