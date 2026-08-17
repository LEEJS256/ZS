// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZS_MinimapIcon.generated.h"

class UImage;
class UTexture;
/**
 * 
 */
UCLASS()
class ZS_API UZS_MinimapIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	// WBP_MinimapIcon 블루프린트에서 이름이 "IconImage"인 Image 위젯과 바인딩됨
	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetIconTexture(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = "Minimap")
	void SetIconRotation(float AngleDegree);
	
};
