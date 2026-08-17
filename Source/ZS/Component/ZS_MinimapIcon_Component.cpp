// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ZS_MinimapIcon_Component.h"

#include "ZS_MinimapSubsystem.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UZS_MinimapIcon_Component::UZS_MinimapIcon_Component()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;


}

// Called when the game starts
void UZS_MinimapIcon_Component::BeginPlay()
{
	Super::BeginPlay();
	AActor* OwnerActor = GetOwner();

	if (UWorld* World = GetWorld())
	{
		if (UZS_MinimapSubsystem* MinimapSubsystem = World->GetSubsystem<UZS_MinimapSubsystem>())
		{
			MinimapSubsystem->RegisterIconComponent(this);
		}
	}
}

void UZS_MinimapIcon_Component::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UZS_MinimapSubsystem* MinimapSubsystem = World->GetSubsystem<UZS_MinimapSubsystem>())
		{
			MinimapSubsystem->UnregisterIconComponent(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}





// Called every frame
void UZS_MinimapIcon_Component::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
