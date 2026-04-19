// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZSPreviewComponent.generated.h"

USTRUCT()
struct FTrajectoryParams
{
	GENERATED_BODY()

	FVector StartLocation;
	FVector LaunchVelocity;
	float Radius = 5.f;
};

class UGA_Right_ATK;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZS_API UZSPreviewComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UZSPreviewComponent();

	void StartPreview(const FTrajectoryParams& InParams);
	void StartPreview_RightGA(UGA_Right_ATK* InGA);
	void StopPreview();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region GA
	UPROPERTY()
	UGA_Right_ATK* RightGA;

#pragma endregion 
public:
	bool bIsPreviewing = false;
	FTrajectoryParams Params;
	// Called every frame

		
};
