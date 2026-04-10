// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/ZSMonsterBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "DA/ZSPlayerDataAsset.h"
#include "Components/WidgetComponent.h"
#include "Controller/ZSAIController.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WBP_Damage_Text.h"
#include "UI/ZS_MonsterHealthWidget.h"
#include "Utility/ZSNativeGameplayTag.h"

// Sets default values
AZSMonsterBase::AZSMonsterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
	AttributeSet = CreateDefaultSubobject<UZSAttributeSet>("AttributeSet");

	AIControllerClass = AZSAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));

	HealthBarComponent->SetupAttachment(RootComponent);

	// GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	// GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);


	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	
}

// Called when the game starts or when spawned
void AZSMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AbilitySystemComponent->AddLooseGameplayTag(TAG_Team_Monster);
		InitializePlayerDA();
	}

	BaseDelegate();
	BaseUI();
	// MoveToNextPoint();
}


void AZSMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 예시
	// DOREPLIFETIME(AZSMonsterBase, Health);
}

// Called every frame
void AZSMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDissolving)
	{
		UpdateDissolve(DeltaTime);
	}

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

void AZSMonsterBase::DeathMonster()
{
	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}

	// 충돌 끄기
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시 충돌도 끄는 게 안전
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 잠깐 남겼다가 삭제 (연출용)
	// SetLifeSpan(3.f);
	ApplyDissolveMI();
	SetDissolve();
	// Destroy();
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

void AZSMonsterBase::UpdateHealthBarRotation()
{
	if (!HealthBarComponent || !HealthBarComponent->IsVisible())
		return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector WidgetLocation = HealthBarComponent->GetComponentLocation();

	// 1 카메라 바라보기
	FVector Direction = CameraLocation - WidgetLocation;
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	HealthBarComponent->SetWorldRotation(LookAtRotation);

	// 거리 기반 스케일 ⭐ (여기 같이 처리)
	float Distance = FVector::Dist(CameraLocation, WidgetLocation);

	float Scale = FMath::Clamp(Distance / 500.f, 0.5f, 1.5f);

	HealthBarComponent->SetWorldScale3D(FVector(Scale));
}

void AZSMonsterBase::BaseDelegate()
{
	//체력변했을때
	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())
		.AddUObject(this, &AZSMonsterBase::OnHealthChanged);

	//데미지 받았을때
	AttributeSet->OnDamageTaken.AddUObject(this, &AZSMonsterBase::HandleDamageTaken);
}

void AZSMonsterBase::BaseUI()
{
	if (HealthBarComponent)
	{
		UZS_MonsterHealthWidget* Widget =
			Cast<UZS_MonsterHealthWidget>(HealthBarComponent->GetUserWidgetObject());

		HealthBarWidget = Widget;
	}

	GetWorldTimerManager().SetTimer(
		RotateHandle,
		this,
		&AZSMonsterBase::UpdateHealthBarRotation,
		0.05f,
		true
	);
}

void AZSMonsterBase::UpdateUI()
{
	if (HealthBarWidget)
	{
		HealthBarComponent->SetVisibility(false);

		float HP = AbilitySystemComponent->GetNumericAttribute(UZSAttributeSet::GetHealthAttribute());
		float MaxHP = AbilitySystemComponent->GetNumericAttribute(UZSAttributeSet::GetMaxHealthAttribute());

		float Ratio = HP / MaxHP;
		HealthBarWidget->SetHealthPercent(Ratio);
	}
}

void AZSMonsterBase::UpdateDissolve(float DeltaTime)
{
	DissolveValue += DeltaTime;

	float Value = FMath::Clamp(DissolveValue / 2.0f, 0.f, 1.f);
	
	// UE_LOG(LogTemp, Error, TEXT("MID_Value:%.2f"),Value);
	for (auto MID : DissolveMIDs)
	{
		MID->SetScalarParameterValue("DissolveAmount", Value);
	}

	if (Value >= 1.f)
	{
		Destroy();
	}
}

void AZSMonsterBase::SetDissolve(bool ArgDissolove)
{
	bIsDissolving = ArgDissolove;
}


void AZSMonsterBase::HandleDamageTaken(float Damage, FVector Location, bool bCritical, FGameplayTag DamageType)
{
	if (!DamageWidgetClass) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	UWBP_Damage_Text* Widget = CreateWidget<UWBP_Damage_Text>(PC, DamageWidgetClass);
	if (!Widget) return;

	Widget->AddToViewport();

	// 🔹 위치 (월드 → 스크린)
	FVector Offset = FVector(
		FMath::RandRange(-30.f, 30.f),
		FMath::RandRange(-30.f, 30.f),
		100.f
	);

	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(
		PC,
		Location + Offset,
		ScreenPos
	);

	Widget->SetPositionInViewport(ScreenPos);

	// 🔹 캐스팅해서 값 전달
	auto DamageWidget = Cast<UWBP_Damage_Text>(Widget);
	if (DamageWidget)
	{
		DamageWidget->InitDamage(Damage, bCritical, DamageType);
	}
}

void AZSMonsterBase::InitializePlayerDA()
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

void AZSMonsterBase::GrantDefaultGA(UZSPlayerDataAsset* Data)
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

void AZSMonsterBase::ApplyDefaultAttributes(UZSPlayerDataAsset* Data)
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

void AZSMonsterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float MaxHealth = AttributeSet->GetMaxHealth();

	float Ratio = NewHealth / MaxHealth;

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Ratio);
	}

	if (HealthBarComponent && !HealthBarComponent->IsVisible())
	{
		HealthBarComponent->SetVisibility(true);
	}

	// if (NewHealth <= 0.f)
	// {
	// 	Die();
	// }
}

void AZSMonsterBase::ApplyDissolveMI()
{
	DissolveMIDs.Empty();

	// for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
	// {
	// 	UMaterialInstanceDynamic* MID =
	// 		GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
	//
	// 	DissolveMIDs.Add(MID);
	// }

	if (!DissolveMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("DissolveMaterial NULL"));
		return;
	}

	for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MID =
			UMaterialInstanceDynamic::Create(DissolveMaterial, this);

		GetMesh()->SetMaterial(i, MID); // ⭐ 이게 핵심

		DissolveMIDs.Add(MID);
	}
		
}