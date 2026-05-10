// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZS_WorldItem.h"
#include "NiagaraComponent.h"
#include "Character/ZSPlayerCharacter.h"
#include "Component/ZS_InventoryComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AZS_WorldItem::AZS_WorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 
	// 줍기 콜리전
	
	
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	PickupCollision->SetSphereRadius(PickupRadius);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
 
	// 나이아가라 이펙트
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	NiagaraEffect->SetupAttachment(RootComponent);
	NiagaraEffect->SetAutoActivate(true);

}

// Called when the game starts or when spawned
void AZS_WorldItem::BeginPlay()
{
	Super::BeginPlay();

	//오버렙할당
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AZS_WorldItem::OnPickupOverlap);
}

void AZS_WorldItem::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PickUp(OtherActor);
}

void AZS_WorldItem::PickUp(AActor* Picker)
{
	if (!ItemData) return;
 
	AZSPlayerCharacter* Player = Cast<AZSPlayerCharacter>(Picker);
	if (!Player) return;
 
	if (!Player->Inventory) return;
 
	bool bSuccess = Player->Inventory->AddItem(ItemData);
	if (bSuccess)
	{
		// 나이아가라 이펙트 중지
		if (NiagaraEffect)
			NiagaraEffect->Deactivate();
 
		Destroy();
	}
}

// Called every frame
void AZS_WorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

