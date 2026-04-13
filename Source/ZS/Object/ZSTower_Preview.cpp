// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSTower_Preview.h"

#include "Controller/ZSPlayerController.h"

AZSTower_Preview::AZSTower_Preview()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AZSTower_Preview::BeginPlay()
{
	Super::BeginPlay();
	if (PreviewMaterial)
	{
		TArray<UStaticMeshComponent*> Meshes = { BaseMesh, RotatingPart, Barrel };

		for (auto Mesh : Meshes)
		{
			if (!Mesh) continue;

			for (int32 i = 0; i < Mesh->GetNumMaterials(); i++)
			{
				UMaterialInstanceDynamic* MID = Mesh->CreateDynamicMaterialInstance(i, PreviewMaterial);
				PreviewMIDs.Add(MID);
			}
		}
		// PreviewMID = BaseMesh->CreateDynamicMaterialInstance(0, PreviewMaterial);
		SetValid(false);
	}
}

void AZSTower_Preview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AZSPlayerController* PC = Cast<AZSPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC) return;

	FVector HitLocation;
	if (PC->GetCenterHitLocation(HitLocation))
	{
		SetActorLocation(HitLocation);
	}
	CheckValid();
}

void AZSTower_Preview::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AZSTower_Preview::SetValid(bool bValid)
{
	bIsValid = bValid;

	if (!PreviewMID) return;

	FLinearColor Color = bValid ? FLinearColor::Green : FLinearColor::Red;
//	PreviewMID->SetVectorParameterValue("M_Color", Color);

	for (auto MID : PreviewMIDs)
	{
		if (MID)
		{
			MID->SetVectorParameterValue("M_Color", Color);
		}
	}
}

void AZSTower_Preview::UpdatePosition()
{
}

void AZSTower_Preview::CheckValid()
{
	TArray<AActor*> Overlaps;
	GetOverlappingActors(Overlaps, AZSTower::StaticClass());

	bIsValid = (Overlaps.Num() == 0);
}

bool AZSTower_Preview::GetValidPosition() const
{
	return	 bIsValid;
}
