// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/ZSPlayerState.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Controller/ZSPlayerController.h"
#include "UI/ZS_StatusWidget.h"
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

void AZSPlayerState::BindToAttributes(AZSPlayerController* PC)
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxStaminaAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetLevelAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetGoldAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetExpAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxExpAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetAttackAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetArmorAttribute())
		.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
	// AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetExpAttribute())
	// 	.AddUObject(PC, &AZSPlayerController::OnAnyAttributeChanged);
	
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

FZS_StatData AZSPlayerState::GetCurrentStatData() const
{
	FZS_StatData Data;

	Data.PlayerName = AttributeSet->Get_PlayerName();
	Data.Level = AttributeSet->GetLevel();
	Data.HP			=  AttributeSet->GetHealth();
	Data.MaxHP		= AttributeSet->GetMaxHealth();
	Data.Stamina	= AttributeSet->GetStamina();
	Data.MaxStamina = AttributeSet->GetMaxStamina();
	Data.Gold		= AttributeSet->GetGold();
	Data.Exp		= AttributeSet->GetExp();
	Data.MaxExp		= AttributeSet->GetMaxExp();
	Data.Attack		= AttributeSet->GetAttack();
	Data.Armor		= AttributeSet->GetArmor();

	return Data;
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

void AZSPlayerState::GrantStateTag_GE(FGameplayTag NewStateTag)
{
	if (!AbilitySystemComponent) return;

	// 이전 State GE 제거
	if (CurrentStateGEHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(CurrentStateGEHandle);
		CurrentStateGEHandle.Invalidate();
	}

	// 새 GE 적용
	if (TSubclassOf<UGameplayEffect>* GEClass = StateTagToGE.Find(NewStateTag))
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(*GEClass, 1.f, Context);
		CurrentStateGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
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
	AttributeSet->SetMaxExp(Data->MaxExp);
	
	AttributeSet->SetHealth(Data->Health);
	AttributeSet->SetMaxHealth(Data->MaxHealth);

	AttributeSet->SetStamina(Data->Stamina);
	AttributeSet->SetMaxStamina(Data->MaxStamina);

	AttributeSet->SetAttack(Data->Attack);
	AttributeSet->SetArmor(Data->Armor);

	AttributeSet->SetSpeed(Data->Speed);
	AttributeSet->SetSprintWeight(Data->SprintWeight);
	AttributeSet->Set_PlayerName(Data->Player_Name);
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
