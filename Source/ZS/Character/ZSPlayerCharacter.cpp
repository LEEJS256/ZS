// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSPlayerCharacter.h"

#include "AbilitySystemInterface.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Component/ZSPreviewComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "PlayerState/ZSPlayerState.h"
#include "UI/ZS_Crosshair.h"
#include "UI/ZS_playerHudWidget.h"
#include "Utility/ZSNativeGameplayTag.h"

// Sets default values
AZSPlayerCharacter::AZSPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	//카메라대로 캐릭터회전
	 bUseControllerRotationYaw = true;
	//bUseControllerRotationYaw = false;

	bUseControllerRotationRoll = false;

	// 메시 Transform 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArm->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;

	PreviewComponent = CreateDefaultSubobject<UZSPreviewComponent>(TEXT("PriviewComponent"));
	
}

UAbilitySystemComponent* AZSPlayerCharacter::GetAbilitySystemComponent() const
{
	// AZSPlayerCharacter* PS = GetPlayerState<AZSPlayerCharacter>();
	// return PS ? PS->GetAbilitySystemComponent() : nullptr;
	return AbilitySystemComponent;
}

void AZSPlayerCharacter::SetSprinting(bool bSprinting)
{
	bIsSprinting = bSprinting;
	float WalkSpeed = AttributeSet->GetSpeed();
	// UE_LOG(LogTemp, Warning, TEXT("SprintWeight: %f"), AttributeSet->GetSprintWeight());
	float SprintSpeed = AttributeSet->GetSprintWeight() * WalkSpeed;
	// UE_LOG(LogTemp, Warning, TEXT("SprintSpeed: %f"), SprintSpeed);
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

// Called when the game starts or when spawned
void AZSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseSetting();
	BaseUI();
}

void AZSPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	InitASCFromPlayerState();
}

// Called every frame
void AZSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrosshair();
}

// Called to bind functionality to input
void AZSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AZSPlayerCharacter::ApplyAttributesToMovement()
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
	{
		return;
	}

	UZSAttributeSet* ZSAttributeSet = PS->GetAttributeSet();
	if (!ZSAttributeSet)
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	MoveComp->MaxWalkSpeed = ZSAttributeSet->GetSpeed();
}

void AZSPlayerCharacter::BaseSetting()
{
	// GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f; // 캐릭터 점프 높이 수정 필요
	GetCharacterMovement()->AirControl = 0.35f;
	// GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	// GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void AZSPlayerCharacter::InitASCFromPlayerState()
{
	AZSPlayerState* PS = GetPlayerState<AZSPlayerState>();
	if (!PS)
		return;
	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	// 초기 속도 적용
	OnSpeedAttributeChanged(FOnAttributeChangeData());

	PS->InitializePlayerDA();
	ApplyAttributesToMovement();
}

void AZSPlayerCharacter::OnSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsValid(AttributeSet))
		return;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp == nullptr)
		return;

	// AttributeSet에서 Speed 값 읽기
	const float NewSpeed = AttributeSet->GetSpeed();

	// 이동 속도 적용
	MoveComp->MaxWalkSpeed = NewSpeed;
}
void AZSPlayerCharacter::UpdateCrosshair()
{
	bool bNowTargeting =IsTargetingEnemy();

	if (bNowTargeting != bPrevTargeting)
	{
		CrosshairWidget->UpdateCrosshairColor(bNowTargeting);
		bPrevTargeting =bNowTargeting;
	}
}


bool AZSPlayerCharacter::IsTargetingEnemy()
{
	bool ReturnResult = false;

	FVector		Start;
	FRotator	Rot;
	Controller->GetPlayerViewPoint(Start,Rot);

	FVector End = Start + Rot.Vector() * 10000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	ReturnResult = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Pawn,
		Params);

	AActor* HitActor = Hit.GetActor();
	
	if (ReturnResult && HitActor)
	{
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(HitActor);
		if (ASCInterface)
		{
			UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
			if (ASC && ASC->HasMatchingGameplayTag(TAG_Team_Monster))
			{
				// 몬스터 맞음
				return true;
			}
		}
	}
	 //
	 // if (ReturnResult && Hit.GetActor()->ActorHasTag("Monster"))
	 // {
	 // }

	
	return false;
}

void AZSPlayerCharacter::BaseUI()
{
	if (HUDWidgetClass)
	{
		HUDWidget = Cast<UZS_playerHudWidget>(CreateWidget(GetWorld(), HUDWidgetClass));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->Init(this);
		}
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UZS_Crosshair>(GetWorld(), CrosshairWidgetClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
		}
	}
}
