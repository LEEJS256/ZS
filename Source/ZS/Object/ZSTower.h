// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "ZSTower.generated.h"

class UZSPlayerDataAsset;
UCLASS()
class ZS_API AZSTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZSTower();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<UZSPlayerDataAsset> ObjectDA;
	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region EFFECT

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* PreviewMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* PreviewMID;
	
#pragma  endregion 

private:
	float AttackRange = 500.f;
	float AttackCooldown = 1.0f;
	float CurrentTime = 0.f;

	void FindAndAttack();
	void Attack(AActor* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* RotatingPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Barrel;
	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UZSAttributeSet* AttributeSet;
	
	void InitializeObjectDA();
	void GrantDefaultGA(UZSPlayerDataAsset* Data);
	void ApplyDefaultAttributes(UZSPlayerDataAsset* Data);

	void SetValid(bool bIsValid = false);
};
