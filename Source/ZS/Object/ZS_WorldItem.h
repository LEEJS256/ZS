// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZS_WorldItem.generated.h"

class UZS_ItemData;
class USphereComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class ZS_API AZS_WorldItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AZS_WorldItem();

	void InitializeWorldItem(UZS_ItemData* InItemData);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma  region Component

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> PickupCollision;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UNiagaraComponent> NiagaraEffect;

#pragma endregion
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	TObjectPtr<UZS_ItemData> ItemData;

	// 줍기 범위 반경
	UPROPERTY(EditAnywhere, Category="Item")
	float PickupRadius = 100.f;


	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent,
	                     AActor* OtherActor,
	                     UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex,
	                     bool bFromSweep,
	                     const FHitResult& SweepResult);

	void PickUp(AActor* Picker);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
