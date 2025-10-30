#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "myHUD.generated.h"

UCLASS()
class MYPROJECT_API UmyHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // --- вже існуюча функція ---
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetAmmo(int32 CurrentAmmo);

    // --- додаємо нову функцію для здоров’я ---
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetHealth(float CurrentHealth);


protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* AmmoBar;
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AmmoText;


    // --- додаємо ще один ProgressBar ---
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;
};
