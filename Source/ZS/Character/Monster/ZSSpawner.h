// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZSSpawner.generated.h"

class ATargetPoint;
UCLASS()
class ZS_API AZSSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZSSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 몬스터 클래스
	UPROPERTY(EditAnywhere, Category="Spawn")
	TSubclassOf<AActor> MonsterClass;

	// 몇 초마다 생성
	UPROPERTY(EditAnywhere, Category="Spawn")
	float SpawnInterval = 2.0f;

	// 최대 생성 개수
	UPROPERTY(EditAnywhere, Category="Spawn")
	int32 MaxSpawnCount = 10;

	int32 CurrentSpawnCount = 0;

	FTimerHandle SpawnTimer;

	void SpawnMonster();

protected:
	UPROPERTY(EditAnywhere, Category="Spawn")
	TArray<ATargetPoint*> Array_TargetPoint;
	
};
