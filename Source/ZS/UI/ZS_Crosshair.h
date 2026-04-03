// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API UZS_Crosshair : public UUserWidget
{
	GENERATED_BODY()

public:

	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCrosshairColor(bool bIsTargeting);
	
protected:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crosshair")
	FLinearColor DefaultColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Crosshair")
	FLinearColor TargetColor;

};
