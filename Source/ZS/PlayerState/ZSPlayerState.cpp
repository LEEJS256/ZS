// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ZSPlayerState.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "AbilitySystemComponent.h"

AZSPlayerState::AZSPlayerState()
{
	InitializeGAS();
}

void AZSPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AZSPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AZSPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UAbilitySystemComponent* AZSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UZSAttributeSet* AZSPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AZSPlayerState::InitializePlayerDA()
{
	if (!HasAuthority())
		return;
	if (!AbilitySystemComponent)
		return;
	UZSPlayerDataAsset* PlayerDA = CharacterData.LoadSynchronous();
	if (!PlayerDA)
		return;

	GrantDefaultGA(PlayerDA);
	ApplyDefaultAttributes(PlayerDA);
}

void AZSPlayerState::GrantDefaultGA(UZSPlayerDataAsset* Data)
{
	if (!Data || !AbilitySystemComponent)
	{
		return;
	}
	
	int32 InputID = 0;
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->StartupGA)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(AbilityClass, 1, InputID, this));
			InputID++;
		}
	}


}

void AZSPlayerState::ApplyDefaultAttributes(UZSPlayerDataAsset* Data)
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

void AZSPlayerState::InitializeGAS()
{
	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UZSAttributeSet>(TEXT("AttributeSet"));
}
