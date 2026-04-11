// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSTower.h"

#include "Character/Monster/ZSMonsterBase.h"
#include "DA/ZSPlayerDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AZSTower::AZSTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	AttributeSet = CreateDefaultSubobject<UZSAttributeSet>("AttributeSet");

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	RootComponent = BaseMesh;

	RotatingPart = CreateDefaultSubobject<UStaticMeshComponent>("RotatingPart");
	RotatingPart->SetupAttachment(BaseMesh);

	Barrel = CreateDefaultSubobject<UStaticMeshComponent>("Barrel");
	Barrel->SetupAttachment(RotatingPart);
	
}

void AZSTower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void AZSTower::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (PreviewMaterial)
	{
		PreviewMID = BaseMesh->CreateDynamicMaterialInstance(0, PreviewMaterial);
	}
	
}

// Called every frame
void AZSTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentTime += DeltaTime;

	if (CurrentTime >= AttackCooldown)
	{
		CurrentTime = 0.f;
		FindAndAttack();
	}

}

void AZSTower::FindAndAttack()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZSMonsterBase::StaticClass(), FoundActors);

	AActor* ClosestTarget = nullptr;
	float MinDist = AttackRange;

	for (AActor* Actor : FoundActors)
	{
		float Dist = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());

		if (Dist < MinDist)
		{
			MinDist = Dist;
			ClosestTarget = Actor;
		}
	}

	if (ClosestTarget)
	{
		Attack(ClosestTarget);
	}
}

void AZSTower::Attack(AActor* Target)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer GATagContainer;
		GATagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Tower.ATK")));

		AbilitySystemComponent->TryActivateAbilitiesByTag(GATagContainer);
	}
}

void AZSTower::InitializeObjectDA()
{
	if (!HasAuthority())
		return;
	if (!AbilitySystemComponent)
		return;
	UZSPlayerDataAsset* MonsterDA = ObjectDA.LoadSynchronous();
	if (!MonsterDA)
		return;

	GrantDefaultGA(MonsterDA);
	ApplyDefaultAttributes(MonsterDA);
}

void AZSTower::GrantDefaultGA(UZSPlayerDataAsset* Data)
{
	if (!Data || !AbilitySystemComponent)
	{
		return;
	}
	
	int32 InputID = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : ObjectDA->StartupGA)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(AbilityClass, 1, InputID, this));
			InputID++;
		}
	}

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	for (const TSubclassOf<UGameplayEffect>& GEClass : ObjectDA->StartupGE)
	{
		if (GEClass)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(
				GEClass->GetDefaultObject<UGameplayEffect>(),
				1.f,
				Ctx
			);
		}
	}
}

void AZSTower::ApplyDefaultAttributes(UZSPlayerDataAsset* Data)
{
	if (!Data || !AttributeSet)
	{
		return;
	}

	AttributeSet->SetHealth(Data->Health);
	AttributeSet->SetMaxHealth(Data->MaxHealth);

	AttributeSet->SetStamina(Data->Stamina);
	AttributeSet->SetMaxStamina(Data->MaxStamina);

	AttributeSet->SetAttack(Data->Attack);
	AttributeSet->SetArmor(Data->Armor);

	AttributeSet->SetSpeed(Data->Speed);
}

void AZSTower::SetValid(bool bIsValid)
{
	if (!PreviewMID) return;

	if (bIsValid)
	{
		PreviewMID->SetVectorParameterValue("M_Color", FLinearColor::Green);
	}
	else
	{
		PreviewMID->SetVectorParameterValue("M_Color", FLinearColor::Red);
	}

}

