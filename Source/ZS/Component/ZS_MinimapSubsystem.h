// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Component/ZS_MinimapIcon_Component.h"
#include "ZS_MinimapSubsystem.generated.h"


class UZS_MinimapIcon_Component;

// 마커 등록/해제 알림 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinimapIconRegistered, UZS_MinimapIcon_Component*, IconComp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinimapIconUnregistered, UZS_MinimapIcon_Component*, IconComp);

UCLASS()
class ZS_API UZS_MinimapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterIconComponent(UZS_MinimapIcon_Component* Comp);
	void UnregisterIconComponent(UZS_MinimapIcon_Component* Comp);

	const TSet<TWeakObjectPtr<UZS_MinimapIcon_Component>>& GetActiveIconComponents() const { return ActiveIconComponents; }
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Minimap")
	FOnMinimapIconRegistered OnIconRegistered;

	UPROPERTY(BlueprintAssignable, Category = "Minimap")
	FOnMinimapIconUnregistered OnIconUnregistered;

private:
	// 컴포넌트가 파괴될 때 dangling 방지를 위해 TWeakObjectPtr 사용
	UPROPERTY()
	TSet<TWeakObjectPtr<UZS_MinimapIcon_Component>> ActiveIconComponents;
};
