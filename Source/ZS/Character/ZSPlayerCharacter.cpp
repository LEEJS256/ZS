// Fill out your copyright notice in the Description page of Project Settings.


#include "ZSPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Attribute/ZSAttributeSet.h"
#include "PlayerState/ZSPlayerState.h"

// Sets default values
AZSPlayerCharacter::AZSPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 메시 Transform 설정
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	bReplicates = true;
}

UAbilitySystemComponent* AZSPlayerCharacter::GetAbilitySystemComponent() const
{
	AZSPlayerCharacter* PS = GetPlayerState<AZSPlayerCharacter>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

// Called when the game starts or when spawned
void AZSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	BaseSetting();
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

}

// Called to bind functionality to input
void AZSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZSPlayerCharacter::BaseSetting()
{
	// GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f; // 캐릭터 점프 높이 수정 필요
	GetCharacterMovement()->AirControl = 0.35f;
	// GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetSpeed();
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
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

