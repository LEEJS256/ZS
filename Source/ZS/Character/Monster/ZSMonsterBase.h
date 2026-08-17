// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "ZSMonsterBase.generated.h"

class UWBP_Damage_Text;
class UZSPlayerDataAsset;
class UWidgetComponent;
class UZS_MonsterHealthWidget;
class UZS_MinimapIcon_Component;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void DeathMonster();
	void MoveToNextPoint();
	void SwitchTarget();
	void Set_StartTargetPoint(ATargetPoint* ParaTarget);
	void Set_EndTargetPoint(ATargetPoint* ParaTarget);
	
protected:
	void UpdateHealthBarRotation();
	void BaseDelegate();
	void BaseUI();
	void UpdateUI();
	void DropItem();
	
	FTimerHandle RotateHandle;

#pragma region EFFECT
	void UpdateDissolve(float DeltaTime);
	void ApplyDissolveMI();
	void SetDissolve(bool ArgDissolove = true);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* DissolveMaterial;
	
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DissolveMIDs;

	float DissolveValue = 0.f;
	bool bIsDissolving = false;
	
#pragma endregion

#pragma region UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWBP_Damage_Text> DamageWidgetClass;
	
	void HandleDamageTaken(float Damage,FVector Location,bool bCritical,FGameplayTag DamageType);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UZS_MinimapIcon_Component> MinimapIconComp;
#pragma  endregion 
	
#pragma region  GAS

	
	void InitializeMonsterDA();
	void GrantDefaultGA(UZSPlayerDataAsset* Data);
	void ApplyDefaultAttributes(UZSPlayerDataAsset* Data);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	TSoftObjectPtr<UZSPlayerDataAsset> CharacterData;
	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UZSAttributeSet* AttributeSet;

	#pragma endregion 
	
	UPROPERTY(EditAnywhere, Category = "AI")
	ATargetPoint* StartTargetPoint;

	UPROPERTY(EditAnywhere, Category = "AI")
	ATargetPoint* EndTargetPoint;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* HealthBarComponent;

	UPROPERTY()
	UZS_MonsterHealthWidget* HealthBarWidget;
	

	
	int32 CurrentIndex = 0;

#pragma region Delegate
	void OnHealthChanged(const FOnAttributeChangeData& Data);
#pragma endregion 
};


