// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ZS_ItemData.generated.h"

/**
 * 
 */
// class UPaperSprite;
UCLASS()
class ZS_API UZS_ItemData : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// UPaperSprite* ItemIcon_Sprite;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ITemIcon;

	// 모양 결정짓기
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FIntPoint> ShapeOffsets;
	
};
