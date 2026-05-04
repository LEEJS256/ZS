// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "ZSPlayerCharacter.generated.h"


class UZSAttributeSet;
class UZS_playerHudWidget;
class UZS_Crosshair;
class UZSPreviewComponent;
class UZS_InventoryComponent;
class UZS_ItemData;
UCLASS()
class ZS_API AZSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZSPlayerCharacter();


	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
#pragma region CommonFunction
	void SetSprinting(bool bSprinting);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UZSPreviewComponent* PreviewComponent;

	UPROPERTY(VisibleAnywhere)
	UZS_InventoryComponent* Inventory;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "SK|GAS")
	UZS_ItemData* Test_ItemData;
	
#pragma  endregion

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
#pragma region GAS

	void ApplyAttributesToMovement();
	//충돌이나 속도,운동관련
	void BaseSetting();
	void InitASCFromPlayerState();

	void OnSpeedAttributeChanged(const FOnAttributeChangeData& Data);

	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UZSAttributeSet* AttributeSet;
#pragma endregion

#pragma region UI
	bool IsTargetingEnemy();
	void BaseUI();
	void UpdateCrosshair();

	bool bPrevTargeting = false;
	
	UPROPERTY()
	UZS_playerHudWidget* HUDWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY()
	UZS_Crosshair* CrosshairWidget;
#pragma endregion


	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

private:
#pragma region PlayerAnim
	bool bIsSprinting = false;

#pragma  endregion
};


