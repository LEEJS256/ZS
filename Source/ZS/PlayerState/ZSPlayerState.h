// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZS/DA/ZSPlayerDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "ZSPlayerState.generated.h"

class UAbilitySystemComponent;
class UZSAttributeSet;
class AZSPlayerController;
struct FZS_StatData;
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

	void BindToAttributes(AZSPlayerController* PC);
	// 네트워크 복제에 필요한 함수 재정의
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region GAS-function
	void InitializePlayerDA();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UZSAttributeSet* GetAttributeSet() const;

	FZS_StatData GetCurrentStatData() const;

	void GrantStateTag(FGameplayTag NewStateTag);
#pragma endregion
	
protected:
#pragma region GAS
	void GrantDefaultGA(UZSPlayerDataAsset* Data);
	void ApplyDefaultAttributes(UZSPlayerDataAsset* Data);

	UPROPERTY()
	TArray<FGameplayTag> StateTags;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<UZSPlayerDataAsset> CharacterData;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "SK|GAS")
	class UZSAttributeSet* AttributeSet;
#pragma endregion

private:
	void InitializeGAS();
};


