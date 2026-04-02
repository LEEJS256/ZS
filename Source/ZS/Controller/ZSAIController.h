// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZSAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZS_API AZSAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZSAIController();

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
