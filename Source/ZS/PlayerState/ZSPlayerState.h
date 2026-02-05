// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ZSPlayerState.generated.h"

class UAbilitySystemComponent;
class UZSAttributeSet;
/**
 * 
 */
UCLASS()
class ZS_API AZSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AZSPlayerState();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	// 네트워크 복제에 필요한 함수 재정의
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region GAS-function
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UZSAttributeSet* GetAttributeSet() const;
#pragma endregion
	
protected:
#pragma region GAS

	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UZSAttributeSet* AttributeSet;
#pragma endregion

private:
	void InitializeGAS();
};
