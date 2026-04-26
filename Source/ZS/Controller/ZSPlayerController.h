// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "ZSPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UZS_TotalInformationWidget;
/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AZSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	AZSPlayerController();
	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

#pragma region UI

	UPROPERTY()
	UZS_TotalInformationWidget* TotalInformation_Widget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UZS_TotalInformationWidget> TotalInformation_WidgetClass;

#pragma endregion

public:
	bool GetMouseHitLocation(FVector& OutLocation);
	bool GetCenterHitLocation(FVector& OutLocation);

#pragma region Delegate
	void OnAnyAttributeChanged(const FOnAttributeChangeData& Data);

#pragma endregion

#pragma  region InputMappings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZS|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZS|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> FireProjectileAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> Fire_Right_Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> Fire_Ultimate_Action;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> BuildTower_Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ZS|Input")
	TObjectPtr<UInputAction> Action_Inventory;


#pragma endregion

private:
#pragma  region Action
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void StopJumping();

	void StartSprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void StartFireProjectile(const FInputActionValue& Value);
	void ATK_Right(const FInputActionValue& Value);
	void ATK_Ultimate(const FInputActionValue& Value);
	void BuildTower(const FInputActionValue& Value);
	void Open_Inventory(const FInputActionValue& Value);
#pragma endregion
};
