// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZSGameMode.h"

AZSGameMode::AZSGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultPawnClass = nullptr;
	bUseSeamlessTravel = true;
}

void AZSGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AZSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
