// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Engine/TargetPoint.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "GameFramework/Character.h"
#include "ZSMonsterBase.generated.h"

UCLASS()
class ZS_API AZSMonsterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZSMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void MoveToNextPoint();
	void SwitchTarget();
	void Set_StartTargetPoint(ATargetPoint* ParaTarget);
	void Set_EndTargetPoint(ATargetPoint* ParaTarget);

protected:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UZSAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, Category = "AI")
	ATargetPoint* StartTargetPoint;

	UPROPERTY(EditAnywhere, Category = "AI")
	ATargetPoint* EndTargetPoint;


	int32 CurrentIndex = 0;
};
