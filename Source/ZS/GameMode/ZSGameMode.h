// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZSGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AZSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AZSGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};



