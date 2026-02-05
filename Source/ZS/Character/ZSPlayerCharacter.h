// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "ZSPlayerCharacter.generated.h"


class UZSAttributeSet;

UCLASS()
class ZS_API AZSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZSPlayerCharacter();


	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region GAS

	//충돌이나 속도,운동관련
	void BaseSetting();
	void InitASCFromPlayerState();

	void OnSpeedAttributeChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UZSAttributeSet* AttributeSet;
#pragma endregion


};
