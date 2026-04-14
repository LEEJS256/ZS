// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "ZSTower.generated.h"

class USphereComponent;
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

	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<UZSPlayerDataAsset> ObjectDA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* RotatingPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Barrel;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* DetectionSphere;

	UPROPERTY(EditAnywhere)
	float AttackRange = 1500.f;

	UPROPERTY(EditAnywhere)
	bool bBattle = false;
	
	
private:
	float AttackCooldown = 1.0f;
	float CurrentTime = 0.f;

	void FindAndAttack();
	void Attack();

	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UZSAttributeSet* AttributeSet;
	
	void InitializeObjectDA();
	void GrantDefaultGA(UZSPlayerDataAsset* Data);
	void ApplyDefaultAttributes(UZSPlayerDataAsset* Data);

	UPROPERTY()
	AActor* LockOnTarget = nullptr;
	
	void FindTarget();
	
	void RotatingTower(float DeltaTime);
};
