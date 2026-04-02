// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/ZSSpawner.h"

#include "ZSMonsterBase.h"


// Sets default values
AZSSpawner::AZSSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AZSSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
	SpawnTimer,
	this,
	&AZSSpawner::SpawnMonster,
	SpawnInterval,
	true
);
	
}

// Called every frame
void AZSSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZSSpawner::SpawnMonster()
{
	if (CurrentSpawnCount >= MaxSpawnCount)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		return;
	}

	if (!MonsterClass) return;

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();

	AZSMonsterBase* Spawned = GetWorld()->SpawnActor<AZSMonsterBase>(
		MonsterClass,
		SpawnLocation,
		SpawnRotation
	);

	if (Spawned)
	{
		// AZSMonsterBase* Monster = Cast<AZSMonsterBase>(Spawned);
		// Spawned->SpawnDefaultController(); 
		if (Spawned)
		{
			Spawned->Set_StartTargetPoint(Array_TargetPoint[0]);
			Spawned->Set_EndTargetPoint(Array_TargetPoint[1]);

			Spawned->MoveToNextPoint();
		}
	}

	CurrentSpawnCount++;
}

