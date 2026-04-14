// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/ZSTower.h"
#include "ZSTower_Preview.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API AZSTower_Preview : public AZSTower
{
	GENERATED_BODY()
public:
	AZSTower_Preview();

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool GetValidPosition() const;
protected:
#pragma region EFFECT

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface* PreviewMaterial;
	
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> PreviewMIDs;
	
	void SetValid(bool bValid = false);
	void UpdatePosition();
	void CheckValid();
	
	bool bIsValid = false;
	
#pragma  endregion
	
};
