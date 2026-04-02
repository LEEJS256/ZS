// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/ZSMonsterBase.h"
#include "Controller/ZSAIController.h"
#include "GAS/Attribute/ZSAttributeSet.h"

// Sets default values
AZSMonsterBase::AZSMonsterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	AttributeSet = CreateDefaultSubobject<UZSAttributeSet>("AttributeSet");

	AIControllerClass = AZSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
}

// Called when the game starts or when spawned
void AZSMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	MoveToNextPoint();

}

// Called every frame
void AZSMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AZSMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AZSMonsterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AZSMonsterBase::MoveToNextPoint()
{
	AAIController* AI = Cast<AAIController>(GetController());
	if (!AI) return;

	AActor* Target = (CurrentIndex == 0) ? StartTargetPoint : EndTargetPoint;

	AI->MoveToActor(Target);
}

void AZSMonsterBase::SwitchTarget()
{
	CurrentIndex = (CurrentIndex == 0) ? 1 : 0;

	MoveToNextPoint();
}

void AZSMonsterBase::Set_StartTargetPoint(ATargetPoint* ParaTarget)
{
	StartTargetPoint = ParaTarget;
}

void AZSMonsterBase::Set_EndTargetPoint(ATargetPoint* ParaTarget)
{
	EndTargetPoint = ParaTarget;
}

