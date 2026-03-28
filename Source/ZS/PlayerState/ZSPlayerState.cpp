// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ZSPlayerState.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Utility/ZSNativeGameplayTag.h"

AZSPlayerState::AZSPlayerState()
{
	StateTags = {
		TAG_State_Idle,
		TAG_State_Run
	};
	
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
	GrantStateTag(TAG_State_Idle);
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

	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	for (const TSubclassOf<UGameplayEffect>& GEClass : CharacterData->StartupGE)
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

void AZSPlayerState::GrantStateTag(FGameplayTag NewStateTag)
{
	if (!AbilitySystemComponent)
		return;

	// 기존 State 태그 전부 제거
	for (const FGameplayTag& Tag : StateTags)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
	}

	// 새 상태 태그 추가
	AbilitySystemComponent->AddLooseGameplayTag(NewStateTag);
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
