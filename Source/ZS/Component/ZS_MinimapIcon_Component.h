// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ZS_MinimapIcon_Component.generated.h"

class UTexture2D;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZS_API UZS_MinimapIcon_Component : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZS_MinimapIcon_Component();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TObjectPtr<UTexture2D> IconTexture;

	// 미니맵 UI 상의 아이콘 크기 (기본 16x16)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	FVector2D IconSize = FVector2D(16.f, 16.f);

	// 미니맵 화면 밖으로 나갔을 때 테두리에 고정(Clamp)할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bClampToBorder = false;

	// 아이콘 표시 여부 토글 (은신, 시야 밖 처리 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bIsVisibleOnMinimap = true;

	// 액터의 회전에 맞춰 아이콘도 회전할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	bool bFollowActorRotation = false;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

		
};
