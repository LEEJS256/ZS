// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ZSTower.h"

#include "Character/Monster/ZSMonsterBase.h"
#include "Components/SphereComponent.h"
#include "DA/ZSPlayerDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

	DetectionSphere = CreateDefaultSubobject<USphereComponent>("DetectionSphere");
	DetectionSphere->SetupAttachment(RootComponent);

	DetectionSphere->SetSphereRadius(AttackRange); // 공격 범위

	DetectionSphere->SetGenerateOverlapEvents(true);
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetCollisionObjectType(ECC_WorldDynamic);

	// 몬스터만 감지
	DetectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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
}

// Called every frame
void AZSTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bBattle)
		return;

	RotatingTower(DeltaTime);
	CurrentTime += DeltaTime;
	
	if (CurrentTime >= AttackCooldown)
	{
		CurrentTime = 0.f;
		Attack();
	}

	DrawDebugSphere(
		GetWorld(),
		DetectionSphere->GetComponentLocation(),
		DetectionSphere->GetScaledSphereRadius(), // 구체는 반지름 하나면 충분합니다.
		12,                                       // 구체를 표현할 선(세그먼트)의 개수 (보통 12~26)
		FColor::Green,
		false,
		0.1f,                                     // 지속 시간
		0,
		2.f                                       // 선 두께
	);
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

	// if (ClosestTarget)
	// {
	// 	Attack(ClosestTarget);
	// }
}

void AZSTower::Attack()
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
	UZSPlayerDataAsset* DataAssetPtr = ObjectDA.LoadSynchronous();
	if (!DataAssetPtr)
		return;

	GrantDefaultGA(DataAssetPtr);
	ApplyDefaultAttributes(DataAssetPtr);
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

void AZSTower::FindTarget()
{
	TArray<AActor*> Overlaps;
	//DetectionSphere->GetOverlappingActors(Overlaps);

	DetectionSphere->GetOverlappingActors(Overlaps, AZSMonsterBase::StaticClass());

	float MinDist = FLT_MAX;
	AActor* Target = nullptr;

	for (AActor* Actor : Overlaps)
	{
		if (!Actor->IsA(AZSMonsterBase::StaticClass()))
			continue; // ❗ 몬스터만 통과
		float Dist = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());

		if (Dist < MinDist)
		{
			MinDist = Dist;
			LockOnTarget = Actor;
		}
	}


}

void AZSTower::RotatingTower(float DeltaTime)
{
	FindTarget();
	
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
		RotatingPart->GetComponentLocation(),
		LockOnTarget->GetActorLocation()
	);

	// Yaw
	{
		FRotator Current = RotatingPart->GetComponentRotation();
		FRotator TargetR(0.f, LookAtRot.Yaw, 0.f);
		FRotator NewRot = FMath::RInterpTo(Current, TargetR, DeltaTime, 5.f);

		RotatingPart->SetWorldRotation(NewRot);
	}

	// Pitch
	{
		FRotator Current = Barrel->GetComponentRotation();
		FRotator TargetR(LookAtRot.Pitch, RotatingPart->GetComponentRotation().Yaw, 0.f);
		FRotator NewRot = FMath::RInterpTo(Current, TargetR, DeltaTime, 5.f);

		Barrel->SetWorldRotation(NewRot);
	}
}
