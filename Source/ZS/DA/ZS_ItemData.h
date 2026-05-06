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
	UTexture2D* ItemIcon;

	// 모양 결정짓기
	//(예: 2x3 아이템 → {0,0},{1,0},{0,1},{1,1},{0,2},{1,2})
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FIntPoint> ShapeOffsets;

	UPROPERTY()
	bool bRotated = false;


#pragma  region PublicFunction
	// ShapeOffsets에서 기본 W/H 계산
	int32 GetBaseWidth() const
	{
		int32 W = 0;
		for (const FIntPoint& P : ShapeOffsets) W = FMath::Max(W, P.X + 1);
		return FMath::Max(W, 1);
	}

	int32 GetBaseHeight() const
	{
		int32 H = 0;
		for (const FIntPoint& P : ShapeOffsets) H = FMath::Max(H, P.Y + 1);
		return FMath::Max(H, 1);
	}

	// 회전 반영한 실제 배치 크기
	int32 GetPlacedWidth() const { return bRotated ? GetBaseHeight() : GetBaseWidth(); }
	int32 GetPlacedHeight() const { return bRotated ? GetBaseWidth() : GetBaseHeight(); }

	// 회전된 ShapeOffsets 반환 (충돌 체크용)
	TArray<FIntPoint> GetPlacedOffsets() const
	{
		if (!bRotated) return ShapeOffsets;

		int32 BaseH = GetBaseHeight();
		TArray<FIntPoint> Rotated;
		for (const FIntPoint& P : ShapeOffsets)
		{
			// 90도 시계방향: (x, y) → (BaseH - 1 - y, x)
			Rotated.Add(FIntPoint(BaseH - 1 - P.Y, P.X));
		}
		return Rotated;
	}
#pragma endregion
};
