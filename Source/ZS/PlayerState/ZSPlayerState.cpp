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

void AZSPlayerState::InitializeGAS()
{
	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // or Full

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UZSAttributeSet>(TEXT("AttributeSet"));
}
